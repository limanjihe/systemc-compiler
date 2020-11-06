/******************************************************************************
 * Copyright (c) 2020, Intel Corporation. All rights reserved.
 * 
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.
 * 
 *****************************************************************************/

/**
 * Author: Mikhail Moiseev
 */

#include "sc_tool/expr/ScParseRangeExpr.h"
#include "sc_tool/expr/ScGenerateExpr.h"
#include "sc_tool/diag/ScToolDiagnostic.h"
#include "sc_tool/utils/StringFormat.h"
#include "sc_tool/utils/ScTypeTraits.h"
#include "sc_tool/utils/CppTypeTraits.h"
#include "sc_tool/utils/DebugOptions.h"
#include "sc_tool/ScCommandLine.h"

#include "clang/AST/Decl.h"
#include <sstream>

namespace sc {

using namespace std;
using namespace clang;
using namespace llvm;


// ---------------------------------------------------------------------------

ScGenerateExpr::ScGenerateExpr(
                    const clang::ASTContext& context_, 
                    std::shared_ptr<ScState> state_, const SValue& modval_,
                    ScVerilogWriter* codeWriter_) : 
    ScParseExpr(context_, state_, modval_),
    codeWriter(codeWriter_)
{
    using namespace std;
    using namespace clang;

    // Radix updated for some tuples, it needs more iteration to spread changes
    bool updated = false;
    
    do {
        // Use cloned state as modified in @parseValue and used in @initConstRadix
        ScParseExprValue parseValue(astCtx, shared_ptr<ScState>(state->clone()), 
                                    modval, false, recval);
        
        // Evaluate member integer constants to get radix and store it in state
        updated = state->initConstRadix(parseValue);
        
    } while (updated);
    
    {
        auto typeInfo = getIntTraits(astCtx.CharTy);  
        SCT_TOOL_ASSERT(typeInfo, "No type info found");
        if (typeInfo->first != 8) {
            ScDiag::reportScDiag(ScDiag::CPP_NONSTD_TYPE_WIDTH) << "char";
        }
    }
    {
        auto typeInfo = getIntTraits(astCtx.UnsignedCharTy);  
        SCT_TOOL_ASSERT(typeInfo, "No type info found");
        if (typeInfo->first != 8) {
            ScDiag::reportScDiag(ScDiag::CPP_NONSTD_TYPE_WIDTH) << "char";
        }
    }
    {
        auto typeInfo = getIntTraits(astCtx.ShortTy);  
        SCT_TOOL_ASSERT(typeInfo, "No type info found");
        if (typeInfo->first != 16) {
            ScDiag::reportScDiag(ScDiag::CPP_NONSTD_TYPE_WIDTH) << "char";
        }
    }
    {
        auto typeInfo = getIntTraits(astCtx.UnsignedShortTy);  
        SCT_TOOL_ASSERT(typeInfo, "No type info found");
        if (typeInfo->first != 16) {
            ScDiag::reportScDiag(ScDiag::CPP_NONSTD_TYPE_WIDTH) << "char";
        }
    }
    {
        auto typeInfo = getIntTraits(astCtx.IntTy);  
        SCT_TOOL_ASSERT(typeInfo, "No type info found");
        if (typeInfo->first != 32) {
            ScDiag::reportScDiag(ScDiag::CPP_NONSTD_TYPE_WIDTH) << "int";
        }
    }
    {
        auto typeInfo = getIntTraits(astCtx.UnsignedIntTy);  
        SCT_TOOL_ASSERT(typeInfo, "No type info found");
        if (typeInfo->first != 32) {
            ScDiag::reportScDiag(ScDiag::CPP_NONSTD_TYPE_WIDTH) << "unsigned";
        }
    }
    {
        auto typeInfo = getIntTraits(astCtx.LongTy);  
        SCT_TOOL_ASSERT(typeInfo, "No type info found");
        if (typeInfo->first != 32 && typeInfo->first != 64) {
            ScDiag::reportScDiag(ScDiag::CPP_NONSTD_TYPE_WIDTH) << "long";
        }
    }
    {
        auto typeInfo = getIntTraits(astCtx.UnsignedLongTy);  
        SCT_TOOL_ASSERT(typeInfo, "No type info found");
        if (typeInfo->first != 32 && typeInfo->first != 64) {
            ScDiag::reportScDiag(ScDiag::CPP_NONSTD_TYPE_WIDTH) << "unsigned long";
        }
    }
    {
        auto typeInfo = getIntTraits(astCtx.LongLongTy);  
        SCT_TOOL_ASSERT(typeInfo, "No type info found");
        if (typeInfo->first != 64) {
            ScDiag::reportScDiag(ScDiag::CPP_NONSTD_TYPE_WIDTH) << "long long";
        }
    }
    {
        auto typeInfo = getIntTraits(astCtx.UnsignedLongLongTy);  
        SCT_TOOL_ASSERT(typeInfo, "No type info found");
        if (typeInfo->first != 64) {
            ScDiag::reportScDiag(ScDiag::CPP_NONSTD_TYPE_WIDTH) 
                    << "unsigned long long";
        }
    }     
}

// ---------------------------------------------------------------------------
// Utility functions

// Check if argument is narrowing integral to boolean cast
bool ScGenerateExpr::isIntToBoolCast(const Expr* expr) 
{
    if (auto castExpr = dyn_cast<ImplicitCastExpr>(expr)) {
        auto castKind = castExpr->getCastKind();
        
        if (castKind == CK_IntegralToBoolean) {
            auto argType = castExpr->getSubExpr()->getType();
            
            if (auto typeInfo = getIntTraits(argType, false)) {
                size_t width = typeInfo.getValue().first;
                return (width > 1);
            }
        }
    }
    return false;
}

// Check if argument is SC channel
bool ScGenerateExpr::isArgChannel(const Expr* argExpr) {
    if (const MemberExpr* expr = dyn_cast<MemberExpr>(argExpr)) {
        if (expr->getMemberDecl()) {
            return isScChannel(expr->getMemberDecl()->getType());
        }
    } else 
    if (const UnaryOperator* oper = dyn_cast<UnaryOperator>(argExpr)) {
        if (oper->getOpcode() == UO_Deref) {
            return isScChannel(oper->getType());
        }
    } else 
    if (const CastExpr* expr = dyn_cast<CastExpr>(argExpr)) {
        return isArgChannel(expr->getSubExpr());
    }
    // Another expression type support may be required
    return false;
}

// Check if stmt is wait(n) 
bool ScGenerateExpr::isWaitNStmt(const Stmt* stmt)
{
    if (auto callExpr = dyn_cast<const CallExpr>(stmt)) 
    {
        const FunctionDecl* funcDecl = callExpr->getDirectCallee();
        SCT_TOOL_ASSERT (funcDecl, "No function found for call expression");
        string fname = funcDecl->getNameAsString();
        
        string nsname = "unknown";
        if (auto nsDecl = dyn_cast<NamespaceDecl>(funcDecl->
                          getEnclosingNamespaceContext())) {
            nsname = nsDecl->getNameAsString();
        }
    
        if (nsname == "sc_core" && fname == "wait" && 
            callExpr->getNumArgs() == 1) 
        {
            auto args = callExpr->getArgs();
            return isAnyInteger(args[0]->getType());
        }
    }
    return false;
}

// Returns true is given statement is call of @sct_assert_in_proc_func()
bool ScGenerateExpr::isTemporalAssert(const Stmt* stmt) const
{
    if (auto expr = dyn_cast<CallExpr>(stmt)) {
        
        if (auto funcDecl = expr->getDirectCallee()) {
            std::string fname = funcDecl->getNameAsString();
            return (fname == "sct_assert_in_proc_func");
        }
    }
    return false;
}

// Get string from char* argument
llvm::Optional<std::string> 
ScGenerateExpr::getStringFromArg(Expr* argExpr) {
    
    Expr* expr = argExpr;
    if (auto* castExpr = dyn_cast<ImplicitCastExpr>(expr)) {
        expr = castExpr->getSubExpr();
    } 
    if (auto* strExpr = dyn_cast<clang::StringLiteral>(expr)) {
        std::string res = strExpr->getString();
        return res;
    }
    
    return llvm::None;
}

// Parse and evaluate pointer/time expression as constant integer, operates
// with constants only, no @keepConstVariables/@calcPartSelectArg checked
// \param val  -- integer if returned true or some other evaluated value else
// \return true if expression is evaluated as constant integer
bool ScGenerateExpr::evaluateConstIntNoCheck(Expr* expr, SValue& val)
{
    // Clone state to avoid its modification
    // Use current @recval as it can be called from record constructor
    //cout << "evaluateConstIntPtr expr# " << hex << expr << dec << endl;
    ScParseExprValue parseExprValue(astCtx, shared_ptr<ScState>(state->clone()), 
                                    modval, false, recval);
    bool res = parseExprValue.evaluateConstInt(expr, val, true);
    //cout << "   res " << res << " val " << val << endl; 

    return res;
}

// Parse and evaluate one expression/statement as constant integer, 
// returns @false for expression with references
// \param val  -- integer if returned true or some other evaluated value else
// \return true if expression is evaluated as constant integer
bool ScGenerateExpr::evaluateConstInt(Expr* expr, SValue& val,
                                      bool checkConst)
{
    if (keepConstVariables || calcPartSelectArg) {
        val = NO_VALUE;
        return false;
    }
    
    return evaluateConstIntNoCheck(expr, val);
}

// Evaluate variable dependent range (@hexpr, @lexpr) expression
// Returns @true if non-constant variable is in the range
bool ScGenerateExpr::evaluateRangeExpr(Expr* hexpr, Expr* lexpr)
{
    // Evaluating the range expression
    ScParseRangeExpr parseRange(astCtx, state, modval);
    
    SValue hval;
    parseRange.evaluate(hexpr, hval);
    auto hvars = parseRange.getVariables();
    size_t hvarNum = hvars.size();
    SValue lval;
    parseRange.evaluate(lexpr, lval);
    auto lvars = parseRange.getVariables();
    size_t lvarNum = lvars.size();
    
    /*cout << "simplifyRangeExpr hval = " << hval << endl;
    for (auto i : hvars) {
        cout << "   " << i << endl;
    }

    cout << "simplifyRangeExpr lval = " << lval << endl;
    for (auto i : lvars) {
        cout << "   " << i << endl;
    }*/
    
    // Check high and low index variables are the same
    bool equalVars = (lvarNum == hvarNum);
    if (equalVars) {
        for (auto h = hvars.begin(); h != hvars.end(); ++h) {
            bool found = false;
            for (auto l = lvars.begin(); l != lvars.end(); ++l) {
                if (*h == *l) {
                    found = true; 
                    lvars.erase(l);
                    break;
                }
            }
            equalVars = equalVars && found;
        }
    }
    equalVars = equalVars && lvars.empty();
    
    if (equalVars) {
        SCT_TOOL_ASSERT (hval.isInteger() && lval.isInteger(), 
                         "No integer in range");
        //cout << "varNum " << hvarNum << endl;
        
        if (hvarNum == 0) {
            // Store calculated values to indices
            codeWriter->putLiteral(hexpr, hval);
            codeWriter->putLiteral(lexpr, lval);
            return false;
            
        } else {
            // Parse lo index expression and put it to hi index expression
            SValue val;
            chooseExprMethod(lexpr, val);
            codeWriter->copyTerm(lexpr, hexpr);
            // Put delta to lo index expression
            APSInt hint; APSInt lint;
            adjustIntegers(hval.getInteger(), lval.getInteger(), hint, lint);
            APSInt delta = hint - lint;
            delta.operator ++();    // plus 1
            SValue rval(delta, 10);
            codeWriter->putLiteral(lexpr, rval);
            return true;
        }
    } else {
        ScDiag::reportScDiag(hexpr->getSourceRange().getBegin(), 
                             ScDiag::SC_RANGE_DIFF_VARS);
        // Required to prevent more errors
        SCT_INTERNAL_FATAL_NOLOC ("Incorrect range error");
    }
    return false;
}

// Get time argument(s) of get_lo_time() and return lo/hi time pair
/*llvm::Optional<std::pair<int, int>> ScGenerateExpr::getTimeFromCall(Expr* expr) 
{
    std::pair<int, int> times;
    expr->dumpColor();
    
    if (CallExpr* callExpr = dyn_cast<CallExpr>(expr)) {
        unsigned agrNum = callExpr->getNumArgs();
        SCT_TOOL_ASSERT(agrNum == 1 || agrNum == 2, "Incorrect argument number");

        bool first = true;
        for (Expr* argExpr : callExpr->arguments()) {
            SValue val; 
            evaluateConstInt(argExpr, val, true);
            
            if (val.isInteger()) {
                int time = val.getInteger().getExtValue();
                if (first) {
                    times.first = time; first = false;
                }
                times.second = time;
                
            } else {
                return llvm::None;
            }
        }
    }
    return times;
}*/

// Parse SCT_ASSERT in module scope and put assertion string into codeWriter
// \return statement for which assertion string is stored
const clang::Stmt* ScGenerateExpr::parseSvaDecl(const clang::FieldDecl* fdecl) 
{
    Expr* expr = removeExprCleanups(fdecl->getInClassInitializer());
    
    std::vector<Expr*> args;
    if (CXXConstructExpr* ctorExpr = dyn_cast<CXXConstructExpr>(expr)) {
        for (auto arg : ctorExpr->arguments()) {
            args.push_back(arg);
        }
    }
    SCT_TOOL_ASSERT (args.size() == 5 || args.size() == 6, 
                     "Incorrect argument number");

    // Set @parseSvaArg to check function call there
    codeWriter->setParseSvaArg(true);
    
    SValue fval; SValue sval; 
    chooseExprMethod(args[0], fval);
    chooseExprMethod(args[1], fval);
    chooseExprMethod(args[2], fval);
    assertName = getStringFromArg(args[3]);
    
    evaluateConstIntNoCheck(args[4], fval);
    if (args.size() == 6) {
        evaluateConstIntNoCheck(args[5], sval);
    } else {
        sval = fval;
    }

    if (fval.isInteger() && sval.isInteger()) {
        std::string timeStr = parseSvaTime(fval.getInteger().getExtValue(), 
                                           sval.getInteger().getExtValue());
        codeWriter->putTemporalAssert(expr, args[0], args[1], timeStr, args[2]);

    } else {
        ScDiag::reportScDiag(expr->getBeginLoc(), 
                             ScDiag::SYNTH_SVA_INCORRECT_TIME);
    }
    codeWriter->setParseSvaArg(false);
    
    return expr;
}

// Generate function parameter assignments
void ScGenerateExpr::prepareCallParams(clang::Expr* expr, 
                                       const SValue& funcModval, 
                                       const clang::FunctionDecl* callFuncDecl) 
{
    bool isCall = isa<CallExpr>(expr);
    bool isCtor = isa<CXXConstructExpr>(expr);
    SCT_TOOL_ASSERT (isCall || isCtor, "No function call or constructor");

    auto callExpr = isCall ? dyn_cast<CallExpr>(expr) : nullptr;
    auto ctorExpr = isCtor ? dyn_cast<CXXConstructExpr>(expr) : nullptr;

    unsigned paramIndx = 0;
    unsigned paramNum = callFuncDecl->getNumParams();
    auto arguments = isCall ? callExpr->arguments() : ctorExpr->arguments();

    for (auto arg : arguments) {
        SCT_TOOL_ASSERT (paramIndx < paramNum, "Incorrect parameter index");
        
        // Remove @ExprWithCleanups from @CXXConstructExpr
        auto argExpr = removeExprCleanups(arg);
        // Check for copy constructor to use record RValue
        auto argCtorExpr = getCXXCtorExprArg(argExpr);
        if (argCtorExpr) {
            recCopyCtor = isCXXCopyCtor(argCtorExpr);
            
            if (!recCopyCtor) {
                ScDiag::reportScDiag(expr->getBeginLoc(), 
                         ScDiag::SYNTH_NONTRIVIAL_COPY);
            }
        }
        
        ParmVarDecl* parDecl = const_cast<ParmVarDecl*>(
                               callFuncDecl->getParamDecl(paramIndx++));

        // Get original type, required as array passed to function as pointer
        QualType type = parDecl->getType();
        if (isScPort(type)) {
            ScDiag::reportScDiag(expr->getBeginLoc(), 
                        ScDiag::SYNTH_SC_PORT_INCORRECT) << "in function parameter";
        }
        bool isRef = isReference(type);
        bool isPtr = isPointer(type);
        bool isArray = type->isArrayType();
        bool isConst = (isRef) ? type.getNonReferenceType().
                        isConstQualified() : type.isConstQualified();
        bool isRecord = isUserDefinedClass(type);

        if (auto dtype = dyn_cast<DecayedType>(type)) {
            type = dtype->getOriginalType();
        }
        
        // Fill state with array elements and initialization values if 
        // variable is constant type, use @arg as initialization expression
        // Parameter variable, can be array variable
        // Use module @funcModval where function is called
        SValue pval = parseValueDecl(parDecl, funcModval, arg, true);
        //arg->dumpColor();
        
        // Parse argument to fill @terms and kill sub-statements
        SValue ival;
        chooseExprMethod(arg, ival);
        
        // For non-reference and constant reference try to evaluate integer,
        // there is temporary variable for constant reference, so bit/range supported
        SValue iival = ival;
        if (!isRecord && (!isRef || isConst)) {
            if (ival.isInteger()) {
                // Put constant integer for the initializer expression
                codeWriter->putLiteral(arg, ival);
            } else 
            if (evaluateConstInt(arg, iival, true)) {
                // Do not put literal
            } else {
                // @iival changed inside of evaluateConstInt() 
                iival = ival;
            }
        }
        
        if (isRef) {
            //cout << "Ref pval " << pval << " ival " << ival << " iival " << iival << endl;
            
            const QualType& refType = type.getNonReferenceType();
            // Check channel has the same type as constant reference parameter
            bool isChanRef = ival.isScChannel() && ival.getScChannel()->
                             getType()->getCanonicalTypeInternal() == 
                             refType->getCanonicalTypeInternal();
            
            // Reference can be to array element or dynamic object
            if ( !(iival.isVariable() || iival.isTmpVariable() || 
                   iival.isArray() || iival.isSimpleObject() || isChanRef) )
            {
                // Constant reference refers to RValue, special variable created
                SCT_TOOL_ASSERT (!isRecord, "Unexpected record initialization");
                SCT_TOOL_ASSERT (isConst, "Incorrect reference initializer");
                
                // Put constant reference to itself to use variable name in code
                codeWriter->storeRefVarDecl(pval, pval);
                // Declare constant reference variable w/o initialization,
                // always declare parameter as range/bit selection can be used
                codeWriter->putVarDecl(nullptr, pval, refType, nullptr, false);
                // Add initialization with the argument corresponds to declaration
                codeWriter->putFCallParam(expr, pval, arg);
                //cout << "prepareCallParams ConstRef pval " << pval << " iival " << iival << endl;
                
            } else {
                // Non-constant or constant reference refers to LValue
                // It is replaced with @arg string in generated code
                codeWriter->storeRefVarDecl(pval, arg);
                
                // Store indices string for array record passed by reference
                bool isRecordRef = isUserDefinedClass(refType);
                if (isRecordRef) {
                    string s = codeWriter->getIndexString(ival);
                    recarrRefIndices[pval] = s;
                    //cout << "prepareCallParams for isRecordRef pval " << pval 
                    //     << " ival " << ival << " indxStr " << s << endl;
                }
            }

        } else 
        if (isPtr || isArray) {
            // For pointers and arrays (really array always converted to pointer)
            // Check if argument is pointer array at unknown index
            bool unkwIndex;
            bool isArr = state->isArray(ival, unkwIndex);
            
            if (isArr) {
                // It is replaced with @arg string in generated code
                codeWriter->storePointerVarDecl(pval, arg);
                
            } else {
                // Try to get channel value from pointe
                SValue cval = getChannelFromState(ival);
                codeWriter->storePointerVarDecl(pval, ival, cval);
            }

        } else {
            // Variable declaration w/o initialization
            codeWriter->putVarDecl(nullptr, pval, type, nullptr);

            // Add parameter initialization with the argument
            if (isRecord) {
                // Get field initialization string from @CXXConstructExpr
                if (argCtorExpr) {
                    codeWriter->addTerm(argCtorExpr, expr);
                }
                
            } else {
                codeWriter->putFCallParam(expr, pval, arg);
            }
        }
    }
    
    // Put empty string to ensure there is term for @expr, required for call
    // in right part of &&\||
    if (!codeWriter->getStmtString(expr)) {
        codeWriter->putEmptyFCallParam(expr);
    }
    
    // Print constructor parameters before its body to insert initialization list 
    // between them, for normal function body graph tied to parameters expression
    if (isCtor) {
        storeStmtStrNull(expr);
    }
}

// Get vector with parent record values for given field variable or record value
// For record array it get record value from its most inner zero element
// \param val -- record field variable or record value
// \return -- vector with record values in hierarchy
std::vector<SValue> ScGenerateExpr::getRecVector(const SValue& val) 
{
    //cout << "getRecVector for val " << val << " : " << endl;
    std::vector<SValue> recarrs;
    
    // Get parent record value
    SValue parval;
    if (val.isVariable()) {
        // @val is record field variable
        parval = val.getVariable().getParent();
    } else 
    if (val.isRecord()) {
        // @val is parent record 
        parval = val;
    }
        
    if (parval.isRecord()) {
        // Fill vector of parent record variables, check @synmodval to avoid
        // hangs up for MIF process accessing parent module fields
        //cout << " parval " << parval << ", modval " << modval 
        //     << " synmodval " << synmodval << endl;
        while (parval && parval != modval && parval != synmodval) {
            SValue recvar = state->getVariableForValue(parval);
            //cout << " parval " << parval << ", recvar " << recvar << endl;
            if (!recvar.isVariable()) break;
            
            //cout << "  recvar " << recvar << endl;
            SValue recarr = state->getValue(recvar);
            // Find most inner array object as it is used in @arraySubIndices
            SValue aval = state->getValue(recarr);
            while (aval.isArray()) {
                recarr = aval;
                aval = state->getValue(recarr);
            }
            recarrs.push_back(recarr);
            
            // Go upper along hierarchy 
            parval = recvar.getVariable().getParent();
            //cout << "  parval " << parval << endl;
        }
        //cout << endl;
    }
    //state->print();
    return recarrs;
}

// Put any member expression specified in @val for @stmt 
void ScGenerateExpr::putMemberExpr(const Expr* expr, const SValue& val, 
                                   const string& refRecarrIndxStr) 
{
    // Get parent record value
    SValue tval;
    if (val.isVariable()) {
        tval = val.getVariable().getParent();
    }
    tval = getRecordFromState(tval, ArrayUnkwnMode::amNoValue);
    // Get record values vector to create name suffix for record member
    auto recvecs = getRecVector(val);

    //cout << "putMemberExpr val " << val << ", recval " << recval 
    //     << ", modval " << modval << ", recvars size " << recvecs.size() 
    //     << ", tval " << tval << endl;
    //state->print();
    
    // Check current MIF array element as a parent of the member
    bool elemOfMifArr = val.isVariable() && 
                        state->checkRecord(val, codeWriter->getMIFName().first,
                                           ScState::MIF_CROSS_NUM);
    // Check access to member of record/MIF from this record/MIF called method
    bool elemOfRecArr = tval && tval == codeWriter->getRecordName().first;
    //cout << "elemOfMifArr " << elemOfMifArr << " elemOfRecArr " << elemOfRecArr << endl;
    
    SValue cval = getChannelFromState(val);
    
    if (cval) {
        // Put channels to provide different read/write names
        codeWriter->putChannelExpr(expr, cval, recvecs, elemOfMifArr, 
                                   elemOfRecArr);
    } else {
        codeWriter->putValueExpr(expr, val, recvecs, elemOfMifArr, 
                                 elemOfRecArr, refRecarrIndxStr);
    }
}


// Put any pointer variable de-referenced string, including pointer parameter
// Used everywhere pointer de-referenced or pointer method called
void ScGenerateExpr::putPtrDerefExpr(const Expr* expr, 
                                     const SValue& rvar,
                                     const SValue& rval, 
                                     const SValue& val)
{
    if (!codeWriter->putLocalPtrValueExpr(expr, rval)) {
        //cout << "putPtrDerefExpr rvar " << rvar << ", rval " << rval << ", val " << val << endl;

        SValue rrval;
        if (val.isVariable() || val.isTmpVariable()) {
            // Use pointed variable/object name
            rrval = val;
            
        } else 
        if (rval.isVariable() || rval.isTmpVariable()) {
            // Use pointer variable name for dynamic object
            SCT_TOOL_ASSERT (false, "Apply putPtrDerefExpr for @lval is variable");
            rrval = rval;
            
        } else 
        if (rval.isObject() && rvar.isVariable()) {
            // Use variable to pointer variable, required for array of pointers
            rrval = rvar;
            
        } else {
            // Do nothing, that could be @this object at method call
        }

        if (!rrval.isUnknown()) {
            // Put expression into code writer
            putMemberExpr(expr, rrval, "");
        }
    }
}

// ---------------------------------------------------------------------------
// Parse statement functions

// Integer literal
void ScGenerateExpr::parseExpr(IntegerLiteral* expr, SValue& val) 
{
    ScParseExpr::parseExpr(expr, val);
    codeWriter->putLiteral(expr, val);
}

// Boolean literal
void ScGenerateExpr::parseExpr(CXXBoolLiteralExpr* expr, SValue& val) 
{
    ScParseExpr::parseExpr(expr, val);
    codeWriter->putLiteral(expr, val);
}

// @nullptr literal
void ScGenerateExpr::parseExpr(CXXNullPtrLiteralExpr* expr, SValue& val) 
{
    ScParseExpr::parseExpr(expr, val);
    codeWriter->putLiteral(expr, val);
}

// Used for local variables access in left/right parts
void ScGenerateExpr::parseExpr(DeclRefExpr* expr, SValue& val)
{
    ScParseExpr::parseExpr(expr, val);
    
    refRecarrIndx = "";
    
    // Non-integer expression/variable replaced by integer, type cast required
    bool replacedByValue = false;    
    // Do de-reference for reference and pointer type 
    if (val.isReference()) {
        // Try to put local reference
        if (codeWriter->putLocalRefValueExpr(expr, val)) {
            
            auto i = recarrRefIndices.find(val);
            if (i != recarrRefIndices.end()) {
                refRecarrIndx = i->second;
            }
            //cout << "DeclRefExpr val " << val << " " << refRecarrIndx << endl;
            //state->print();
             
            return; 
        }
        // Module/global reference
        SValue lval(val);
        // Put referenced variable into @val
        state->getValue(lval, val, false);  
        
    } else 
    if (val.isPointer()) {
        // Try to put local pointer value
        if (codeWriter->putLocalPtrValueExpr(expr, val)) {
            return;
        }
        // Module/global pointer
        
    } else {
        // Non-reference and non-pointer
        // Try to evaluate expression as integer constant
        SValue ival;
        if (evaluateConstInt(expr, ival, true)) {
            if (!val.isInteger()) replacedByValue = true;
            val = ival;
        }
    }
    
    // Local variable declared w/o record prefix, no @recvecs and no @tval used
    codeWriter->putValueExpr(expr, val);
    // Add type cast for replaced integer, required for concatenation        
    if (replacedByValue) {
        codeWriter->setReplacedCastWidth(expr, expr->getType());
    }
}

// Any access of member variable
void ScGenerateExpr::parseExpr(MemberExpr* expr, SValue& val) 
{
    // Set @keepArrayIndices to do not erase them from @arraySubIndices
    // required for array of channel/record/MIF with inner record
    QualType baseType = expr->getBase()->getType();
    bool setSaveArrayIndices = false;

    if (isUserDefinedClass(baseType, true) && !codeWriter->isKeepArrayIndices())
    {
        codeWriter->setKeepArrayIndices();
        setSaveArrayIndices = true;
    }
    
    ScParseExpr::parseExpr(expr, val);
    
    if (setSaveArrayIndices) {
        codeWriter->resetKeepArrayIndices();
    }
    
    // Non-integer expression/variable replaced by integer, type cast required
    bool replacedByValue = false;    
    // Do de-reference for reference type 
    if (val.isReference()) {
        // Try to put local reference
        if (codeWriter->putLocalRefValueExpr(expr, val)) {
            return;
        }
        // Module/global reference
        SValue lval(val);
        // Put referenced variable into @val
        state->getValue(lval, val, false);
        
    } else 
    if (val.isPointer()) {
        // Do not try to put pointer value as it is not local one
        
    } else {
        // Non-reference and non-pointer
        // Try to evaluate expression as integer constant
        SValue ival;
        if (evaluateConstInt(expr, ival, true)) {
            if (!val.isInteger()) replacedByValue = true;
            val = ival;
        }
    }
    
    // Put expression into code writer
    putMemberExpr(expr, val, refRecarrIndx);
    // Add type cast for replaced integer, required for concatenation        
    if (replacedByValue) {
        codeWriter->setReplacedCastWidth(expr, expr->getType());
    }
    // Clear indices suffix after use
    refRecarrIndx = "";
    
    //cout << "ScGenerateExpr::parseExpr(MemberExpr*) >>> #" << hex << expr << dec << endl;
}

// Set root to determine module hierarchy
void ScGenerateExpr::parseExpr(CXXThisExpr* expr, SValue& val)
{
    ScParseExpr::parseExpr(expr, val);
    // No terms should be added here
}

// Used for explicit/implicit type cast and LValue to RValue cast
void ScGenerateExpr::parseExpr(ImplicitCastExpr* expr, SValue& val)
{
    // Parse sub-expression is inside
    ScParseExpr::parseExpr(expr, val);
    auto castKind = expr->getCastKind();

    // Checking cast pointer to boolean and substitute literal if possible
    if (castKind == CK_MemberPointerToBoolean || castKind == CK_PointerToBoolean) {
        // Convert pointer to boolean, true if pointer value is object
        // Do not check pointer is constant as pointer arithmetic is prohibited
        SValue rval = getValueFromState(val);
        codeWriter->putLiteral(expr, SValue(SValue::boolToAPSInt(
                                            rval.isObject()), 10));
        return;
        
    } else 
    if (castKind == CK_IntegralToBoolean) {
        // Convert integer to boolean for literal and variable
        if (val.isInteger()) {
            val = SValue(SValue::boolToAPSInt(val.getBoolValue()), 10);
            codeWriter->putLiteral(expr, val);
            return;
            
        } else {
            // OR reduction required to preserve C++ semantic of boolean type
            string opcodeStr = "|";
            // Add brackets for complex expression
            if (!val.isVariable()) {
                codeWriter->copyTermInBrackets(expr->getSubExpr(), 
                                               expr->getSubExpr());
            }
            codeWriter->putUnary(expr, opcodeStr, expr->getSubExpr(), true);
            return;
        }
    } else
    // May be some other cast types should be added
    if (castKind == CK_IntegralCast || castKind == CK_NoOp) {
        
        QualType type = expr->getType();
        QualType origType = expr->getSubExpr()->getType();
        bool isBool = origType->isBooleanType();
        bool isSigned = isSignedType(type);
        bool isOrigSigned = isSignedType(origType);

        if (val.isInteger()) {
            // Reduce literal for narrowing cast (required for CK_IntegralCast)
            auto typeInfo = getIntTraits(type, true);
            SCT_TOOL_ASSERT (typeInfo, "No integral type width extracted");
            size_t width = typeInfo.getValue().first;
            bool isUnsigned = typeInfo.getValue().second;
        
            val = SValue(extrOrTrunc(val.getInteger(), width, isUnsigned), 
                         val.getRadix());
            
            codeWriter->putLiteral(expr, val);
            
        } else {
            // No type cast required, in Verilog RHS implicitly narrowed to LHS
            codeWriter->copyTerm(expr->getSubExpr(), expr);
        }
        
        // Set/reset sign cast for literals and expressions
        // Skip boolean type conversion as it considered as unsigned
        // No special rule for @enum required, unsigned @enum casted to @int
        if (!isBool && isOrigSigned != isSigned) { 
            codeWriter->putSignCast(expr, isSigned ? CastSign::SCAST : 
                                                     CastSign::UCAST);
        }
        return;
    }
    
    codeWriter->copyTerm(expr->getSubExpr(), expr);
}

void ScGenerateExpr::parseExpr(ExplicitCastExpr* expr, SValue& val)
{
    ScParseExpr::parseExpr(expr, val);
    QualType type = expr->getType();
    QualType origType = expr->getSubExpr()->getType();
    
    // Partial select applied only for C++ or SC integral type
    if (isVoidType(type) && (isa<CStyleCastExpr>(expr) || 
        isa<CXXFunctionalCastExpr>(expr) || isa<CXXStaticCastExpr>(expr))) {
        // Explicit cast to void, remove it from code
        // Used to replace assert in NDEBUG mode, see assert.h
        // Do not put anything for @expr
            
    } else {
        if (getIntTraits(type, true) && !isBoolType(type) &&
            (isa<CStyleCastExpr>(expr) || isa<CXXFunctionalCastExpr>(expr) || 
             isa<CXXStaticCastExpr>(expr))) {
            // Not required to support boolean converting as it is done 
            // in underlying implicit cast
            // Type cast with partial select possible
            codeWriter->putTypeCast(expr->getSubExpr(), expr, type);

        } else {
            // No partial select here
            codeWriter->copyTerm(expr->getSubExpr(), expr);
        }

        bool isBool = origType->isBooleanType();
        bool isSigned = isSignedType(type);
        bool isOrigSigned = isSignedType(origType);

        // Set/reset sign cast for literals and expressions
        // Skip boolean type conversion as it considered as unsigned
        if (!isBool && isOrigSigned != isSigned) { 
            codeWriter->putSignCast(expr, isSigned ? CastSign::SCAST : 
                                                     CastSign::UCAST);
        }
    }
}

// Parenthesized expression, i.e. expression in "()"
void ScGenerateExpr::parseExpr(ParenExpr* expr, SValue& val)
{
    ScParseExpr::parseExpr(expr, val);
    
    // Do not add brackets for concatenation like {a, b}
    bool isConcatSubExpr = false;
    if (auto operExpr = dyn_cast<CXXOperatorCallExpr>(expr->getSubExpr())) {
        OverloadedOperatorKind op = operExpr->getOperator();
        isConcatSubExpr = op == OO_Comma;
    }
    
    if (isConcatSubExpr) {
        codeWriter->copyTerm(expr->getSubExpr(), expr); 
    } else {
        codeWriter->copyTermInBrackets(expr->getSubExpr(), expr); 
    }
}

// Used for default argument value in function calls
void ScGenerateExpr::parseExpr(CXXDefaultArgExpr* expr, SValue& val)
{
    ScParseExpr::parseExpr(expr, val);
    codeWriter->copyTerm(expr->getExpr(), expr);
}

// Used for default initializer in constructor or in aggregate initialization
void ScGenerateExpr::parseExpr(CXXDefaultInitExpr* expr, SValue& val)
{
    // Cannot be called from process
    ScDiag::reportScDiag(expr->getSourceRange().getBegin(), 
                         ScDiag::SC_CXX_INIT_IN_PROC_FUNC);
}

// Used for default initializer in constructor or in aggregate initialization
void ScGenerateExpr::parseExpr(InitListExpr* expr, SValue& val)
{
    QualType type = expr->getType();
    
    if (type->isArrayType()) {
        // TODO : ...
        
    } else {
        if (expr->getNumInits() == 1) {
            val = evalSubExpr(expr->getInit(0));
        } else {
            ScDiag::reportScDiag(expr->getBeginLoc(), 
                ScDiag::SYNTH_UNSUPPORTED_INIT) << " unexpected number of values";
        }
    }
}

// CXX constructor, including @sc_in/@sc_out/@sc_signal/@sc_uint/...
void ScGenerateExpr::parseExpr(CXXConstructExpr* expr, SValue& val)
{
    val = NO_VALUE;
    QualType type = expr->getType();
    
    if (isAnyScInteger(type)) {
        // @sc_uint, @sc_biguint, @sc_int, @sc_bigint
        if (expr->getNumArgs() == 0) {
            // No parameters, constructor initialize object with 0
            val = SValue(APSInt(64, isScUInt(type) || isScBigUInt(type)), 10);
            codeWriter->putLiteral(expr, val);
            
        } else 
        if (expr->getNumArgs() == 1) {
            // Parse initialization expression, it can have arbitrary type
            Expr* iexpr = expr->getArg(0);
            SValue rval; chooseExprMethod(iexpr, rval);
            
            QualType origType = iexpr->getType();
            bool isBool = origType->isBooleanType();
            bool isSigned = isSignedType(type);
            bool isOrigSigned = isSignedType(origType);
            
            // No type cast required, in Verilog RHS implicitly narrowed to LHS
            codeWriter->copyTerm(iexpr, expr); 
            
            // Skip boolean type conversion as it considered as unsigned
            if (!isBool && isOrigSigned != isSigned) { 
                codeWriter->putSignCast(expr, isSigned ? CastSign::SCAST : 
                                                         CastSign::UCAST);
            }
            
        } else {
            SCT_TOOL_ASSERT (false, "Unexpected argument number");
        }
    } else 
    if (isScIntegerArray(type, false)) {
        // Local array of SC type, no in-place initialization supported
        // Do nothing
        
    } else 
    if (isUserDefinedClass(type)) {
        // User defined calls or structure
        if (isScModuleOrInterface(type)) {
            ScDiag::reportScDiag(expr->getBeginLoc(), 
                                 ScDiag::SYNTH_LOCAL_MODULE_DECL);
        }
        
        // Check @locarecvar to skip sub-statement with @CXXConstructExpr
        if (locrecvar) {
            if (recCopyCtor) {
                // Copy constructor to pass record parameter by value
                //cout << "CXXConstructExpr " << hex << expr << dec << endl;
                SCT_TOOL_ASSERT (expr->getArg(0), "No argument in copy constructor");

                // Use copy constructor parameter as result
                SValue rval; chooseExprMethod(expr->getArg(0), rval);
                // At this stage for unknown index zero element returned,
                // so record is always got
                SValue rrec; state->getValue(rval, rrec);
                SCT_TOOL_ASSERT(rrec.isRecord(), "Incorrect value type for "
                                                 "copy constructor");
                // Create record copy 
                val = createRecordCopy(expr, rrec, locrecvar);

                // Get record indices from @arraySubIndices, erased after
                auto rrecvecs = getRecVector(rrec);
                string rrecSuffix = codeWriter->getRecordIndxs(rrecvecs);

                // Assign record fields, LHS indices added inside if need
                codeWriter->putRecordAssign(expr, locrecvar, val, rval, rrec, 
                                            "", rrecSuffix);
            } else {
                // Normal constructor of local record
                // Return created record to assign to its variable
                val = parseRecordCtor(expr, modval, locrecvar, true);
            }
            locrecvar = NO_VALUE;
        }
        recCopyCtor = false;
        
    } else 
    if (getUserDefinedClassFromArray(type)) {
        // Do nothing, record array filled in ScParseExpr::parseValueDecl()
        
    } else {
         ScDiag::reportScDiag(expr->getSourceRange().getBegin(), 
                              ScDiag::SC_CXX_CTOR_IN_PROC_FUNC);
    }
}

// Operator @new and @new[]
void ScGenerateExpr::parseExpr(CXXNewExpr* expr, SValue& val)
{
    // Cannot be called from process
    ScDiag::reportScDiag(expr->getSourceRange().getBegin(), 
                         ScDiag::SC_NEW_IN_PROC_FUNC);
}

// Operator @delete
void ScGenerateExpr::parseExpr(CXXDeleteExpr* expr, SValue& val)
{
    // Cannot be called from process
    ScDiag::reportScDiag(expr->getSourceRange().getBegin(), 
                         ScDiag::SC_DELETE_IN_PROC_FUNC);
}

// Transform temporary that is written to memory so that a reference can bind it
void ScGenerateExpr::parseExpr(MaterializeTemporaryExpr* expr, SValue& val)
{
    ScParseExpr::parseExpr(expr, val);
    codeWriter->copyTerm(getTemporaryExpr(expr), expr); 
}

// Expression with cleanup
void ScGenerateExpr::parseExpr(ExprWithCleanups* expr, SValue& val)
{
    ScParseExpr::parseExpr(expr, val);
    codeWriter->copyTerm(expr->getSubExpr(), expr); 
}

// Represents binding an expression to a temporary.
// Used to call destructor for temporary object
void ScGenerateExpr::parseExpr(CXXBindTemporaryExpr* expr, SValue& val)
{
    ScParseExpr::parseExpr(expr, val);
    codeWriter->copyTerm(expr->getSubExpr(), expr); 
}

// Template parameter use as integer value
void ScGenerateExpr::parseExpr(SubstNonTypeTemplateParmExpr* expr, SValue& val) 
{
    ScParseExpr::parseExpr(expr, val);
    codeWriter->copyTerm(expr->getReplacement(), expr); 
}

// Used for local variable declaration
void ScGenerateExpr::parseDeclStmt(Stmt* stmt, ValueDecl* decl, SValue& val, 
                                   clang::Expr* initExpr)
{
    VarDecl* varDecl = dyn_cast<VarDecl>(decl);
    FieldDecl* fieldDecl = dyn_cast<FieldDecl>(decl);
    SCT_TOOL_ASSERT (varDecl || fieldDecl, "No declaration");
    
    // Use @initExpr if it is given
    bool hasInit = (varDecl && varDecl->hasInit()) || 
                   (fieldDecl && fieldDecl->hasInClassInitializer());
    Expr* iexpr = initExpr ? initExpr : 
                             (hasInit ? (varDecl ? varDecl->getInit() : 
                              fieldDecl->getInClassInitializer()) : nullptr);

    const QualType& type = decl->getType();
    bool isPtr = type->isPointerType();
    bool isRef = type->isReferenceType();
    bool isConst = (isRef) ? type.getNonReferenceType().isConstQualified() : 
                             type.isConstQualified();
    bool isRecord = isUserDefinedClass(type);
    
    if (iexpr) {
        // Remove ExprWithCleanups wrapper
        iexpr = removeExprCleanups(iexpr);
        
        // Do not analyze copy constructor, just assign RValue
        bool isCopyCtor = false;
        if (auto ctorExpr = getCXXCtorExprArg(iexpr)) {
            if (isCXXCopyCtor(ctorExpr)) {
                recCopyCtor = true; 
                isCopyCtor = true;
            }
        }
        // Clear @iexpr to do not assign it in normal record constructor call
        if (isRecord && !isCopyCtor) {
            iexpr = nullptr;
        }
    }
    
    // Use @recval for local record field declarations
    SValue recVal = recval ? recval : modval;
    // Fill state with array elements and initialization values if 
    // variable is constant type
    // Variable or Array variable (i.e. pointer to array)
    // Remove sub-value for non-reference, required to clear state of tuples
    // added at previous analysis of this statement, to get variable for value 
    val = parseValueDecl(decl, recVal, nullptr, true, !isRef);

    if (type->isArrayType()) {
        // Array initialization list
        SCT_TOOL_ASSERT (type->isConstantArrayType(), "Unsupported array type");
        vector<size_t> arrSizes = getArraySizes(type);
        auto recType = getUserDefinedClassFromArray(type);
        
        // Array declaration w/o initialization
        codeWriter->putArrayDecl(stmt, val, type, arrSizes);
        
        if (iexpr) {
            // Only one-dimensional array initialization supported now
            if (arrSizes.size() == 1) {
                if ( auto init = dyn_cast<InitListExpr>(iexpr) ) {
                    for (unsigned i = 0; i < init->getNumInits(); i++) {
                        // Set @i-th element in multi-dimensional array
                        auto arrInds = getArrayIndices(type, i);
                        // Create unsigned 64-bit index value
                        SValue indxVal(APSInt(APInt(64, arrInds[0]), true), 10);
                        // Initializer expression parsed in @parseValueDecl()
                        Expr* iiexpr = const_cast<Expr*>(init->getInit(i));

                        // Parse initializer expression to fill @terms and 
                        // kill sub-statements
                        SValue ival;
                        chooseExprMethod(iiexpr, ival);
                        
                        // Array element initialization assign statement
                        codeWriter->putArrayElemInit(stmt, val, indxVal, iiexpr);
                    }
                } else 
                if ( isScIntegerArray(type, false) ) {
                    // Default initialization SC data type array, fill with zeros
                    for (unsigned i = 0; i < arrSizes.at(0); i++) {
                        // Set @i-th element in multi-dimensional array
                        auto arrInds = getArrayIndices(type, i);
                        // Create unsigned 64-bit index value
                        SValue indxVal(APSInt(APInt(64, arrInds[0]), true), 10);
                        
                        // Array element initialization assign statement
                        codeWriter->putArrayElemInitZero(stmt, val, indxVal);
                    }
                    
                } else 
                if (recType) {
                    // Nothing to add for record array
                    
                } else {
                    string s = iexpr->getStmtClassName();
                    ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                         ScDiag::SYNTH_UNSUPPORTED_INIT) << s;
                }    
            } else {
                ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                     ScDiag::SYNTH_ARRAY_INIT_LIST);
            }
        }
    } else {
        // Single variable 
        
        // SC data type variable has initialization
        SValue ival; SValue iival;
        
        if (iexpr && !isRecord) {
            // Parse initializer expression to fill @terms and kill sub-statements
            chooseExprMethod(iexpr, ival); iival = ival;
            
            // For non-reference and constant reference try to evaluate 
            // expression as constexpr
            if (!isRef || isConst) {
                if (ival.isInteger()) {
                    // Put constant integer for the initializer expression
                    codeWriter->putLiteral(iexpr, ival);
                } else 
                if (evaluateConstInt(iexpr, iival, true)) {
                    // Put literal for loop counter initialization only
                    if (codeWriter->isForLoopInit()) {
                        codeWriter->putLiteral(iexpr, iival);
                    }
                } else {
                    // @iival changed inside of evaluateConstInt() 
                    iival = ival;
                }
            }
        }
        
        if (isRef) {
            //cout << "Ref val " << val << " ival " << ival << " isArgArrayElm " << isArgArrayElm << endl;
            
            const QualType& refType = type.getNonReferenceType();
            
            // Check channel has the same type as constant reference parameter
            bool isChanRef = ival.isScChannel() && ival.getScChannel()->
                             getType()->getCanonicalTypeInternal() == 
                             refType->getCanonicalTypeInternal();
            
            // Reference can be to array element or dynamic object
            if  ( !(iival.isVariable() || iival.isTmpVariable() || 
                    iival.isArray() || iival.isSimpleObject() || isChanRef) )
            {
                // Constant reference refers to RValue, special variable created
                SCT_TOOL_ASSERT (!isRecord, "Unexpected record initialization");
                SCT_TOOL_ASSERT (isConst, "Incorrect reference initializer");

                // Put constant reference to itself to use variable name in code
                codeWriter->storeRefVarDecl(val, val);
                // Declare constant reference variable w/o initialization,
                // always declare parameter as range/bit selection can be used
                codeWriter->putVarDecl(nullptr, val, refType, nullptr, false);
                // Add initialization with the initializer
                codeWriter->putAssign(stmt, val, iexpr);
                
            } else {
                // Non-constant or constant reference refers to LValue
                // It is replaced with @iexpr string in generated code
                codeWriter->storeRefVarDecl(val, iexpr);
            }
            
        } else 
        if (isPtr) {
            // Local pointer variable declaration and initialization
            // Check if argument is pointer array at unknown index
            bool unkwIndex;
            bool isArr = state->isArray(ival, unkwIndex);
            
            if (isArr) {
                // It is replaced with @arg string in generated code
                if (iexpr) {
                    codeWriter->storePointerVarDecl(val, iexpr);
                }
            } else {
                // No local variable pointer to channel supported
                codeWriter->storePointerVarDecl(val, ival, NO_VALUE);
            }
        } else {
            // Single variable declaration with/without initialization,
            // No record declaration/initialization, that is done for its fields            
            if (isRecord) {
                // Get field initialization string from @CXXConstructExpr
                if (iexpr) {
                    codeWriter->copyTerm(iexpr, stmt);
                }
                
            } else {
                codeWriter->putVarDecl(stmt, val, type, iexpr, iival.isInteger());
            }
        }
    }
}

// Parse field declaration without initialization to put into @codeWriter, 
// used in @createRecordCopy
void ScGenerateExpr::parseFieldDecl(ValueDecl* decl, const SValue& lfvar)
{
    //cout << "parseFieldDecl lfvar " << lfvar << " locrecvar " << locrecvar << endl;
    const QualType& type = decl->getType();
    
    // Put field declaration to @localDeclVerilog, @nullptr no initialization
    codeWriter->putVarDecl(nullptr, lfvar, type, nullptr);
}

// Parse array field declaration without initialization to put into @codeWriter, 
// used in declaration array of records
void ScGenerateExpr::parseArrayFieldDecl(ValueDecl* decl, const SValue& lfvar, 
                                         const vector<size_t>& arrSizes)
{
    vector<size_t> allSizes(arrSizes); 
    const QualType& type = decl->getType();

    // Merge record array dimensions and field array dimensions
    if (type->isArrayType()) {
        auto fieldSizes = getArraySizes(type);
        allSizes.insert(allSizes.end(), fieldSizes.begin(), fieldSizes.end());
    }
    
    // Put field declaration to @localDeclVerilog, @nullptr no initialization
    codeWriter->putArrayDecl(nullptr, lfvar, type, allSizes);
}

// Parse statement and run @chooseExprMethod for each operand
void ScGenerateExpr::parseBinaryStmt(BinaryOperator* stmt, SValue& val) 
{
    // There is no value for general binary statement
    val = NO_VALUE;
    Expr* lexpr = stmt->getLHS();
    Expr* rexpr = stmt->getRHS();

    // Operation code
    BinaryOperatorKind opcode = stmt->getOpcode();
    string opcodeStr = stmt->getOpcodeStr(opcode);

    // Skip LHS parsing for comma operator to preserve sub-statement @lexpr 
    // in generated code
    SValue lval; 
    if (opcode != BO_Comma) {
        chooseExprMethod(lexpr, lval);
    }
    
    // Get record indices from @arraySubIndices, it is erased after use
    SValue lrec; state->getValue(lval, lrec);
    string lrecSuffix;
    if (lrec.isRecord()) {
        auto lrecvecs = getRecVector(lrec);
        lrecSuffix = codeWriter->getRecordIndxs(lrecvecs);
    }

    if (opcode == BO_LOr || opcode == BO_LAnd) {
        // Do not consider pointer in condition as it casted to @bool
        // Get condition for term, use @stmt instead of @lexpr as it is term
        SValue flval;
        getTermCondValue(stmt, lval, flval);
        //cout << "BO lexpr " << hex << stmt << dec << " lval " << lval << endl;
        
        if (lval.isInteger() && 
            ((opcode == BO_LOr && !lval.getInteger().isNullValue()) ||
             (opcode == BO_LAnd && lval.getInteger().isNullValue()))) {
            
            val = lval;
            
            // Parse right part to remove sub-statements from code writer
            // Set @skipTerm to ignore (not write) these terms, required as
            // @rexpr term could absent in @terms that leads to error 
            noFuncCall = true;
            codeWriter->setSkipTerm(true);
            SValue rval; 
            chooseExprMethod(rexpr, rval);
            codeWriter->setSkipTerm(false);
            noFuncCall = false;
            
            // Constant condition evaluated
            codeWriter->putValueExpr(stmt, lval);
            
        } else {
            // Parse RValue only if required, 
            // dead condition blocks not analyzed in ScTraverseProc
            SValue rval; 
            chooseExprMethod(rexpr, rval);
            codeWriter->putBinary(stmt, opcodeStr, lexpr, rexpr);
        }
        
    } else {
        SValue rval; 
        chooseExprMethod(rexpr, rval);
        
        // Get record indices from @arraySubIndices, it is erased after use
        SValue rrec; state->getValue(rval, rrec);
        string rrecSuffix;
        if (rrec.isRecord()) {
            auto rrecvecs = getRecVector(rrec);
            rrecSuffix = codeWriter->getRecordIndxs(rrecvecs);
        }
        
        auto ltype = lval.getTypePtr();
        auto rtype = rval.getTypePtr();
        bool lhsPointer = (ltype && ltype->isPointerType());
        bool rhsPointer = (rtype && rtype->isPointerType());

        if (opcode == BO_Assign) {
            // Left value is result of assignment statement
            val = lval;

            // Check pointer as LHS only, pointer as RHS can be assigned to boolean
            if (lhsPointer) {
                // Check pointer type is FILE*
                bool filePtr = false;
                if (auto ptrType = lval.getTypePtr()) {
                    auto objType = ptrType->getPointeeOrArrayElementType();
                    QualType type(objType, 0);
                    filePtr = type.getAsString() == "FILE";
                }
                // No error message for file pointer assignment
                if (!filePtr) {
                    ScDiag::reportScDiag(stmt->getBeginLoc(),
                                         ScDiag::SYNTH_POINTER_OPER) << opcodeStr;
                }
            } else {
                // Assignment with variable/expression in right part
                // Get owner variable, required for arrays
                SValue lvar = state->getVariableForValue(lval);
                // Try to get channel value, used as key in @varTraits 
                SValue cval = getChannelFromState(lvar);
                if (cval) {
                    lvar = cval;
                }
                
                // Put assign for record and others
                bool isRecord = isUserDefinedClass(lval.getType(), true);

                if (isRecord) {
                    codeWriter->putRecordAssign(stmt, lvar, lrec, rval, rrec,
                                                lrecSuffix, rrecSuffix);
                } else {
                    codeWriter->putAssign(stmt, lvar, lexpr, rexpr);
                }
            }
        } else 
        if (opcode == BO_Comma) {
            // C++ operator comma
            val = rval;
            if (lhsPointer || rhsPointer) {
                ScDiag::reportScDiag(stmt->getBeginLoc(), 
                                     ScDiag::SYNTH_POINTER_OPER) << opcodeStr;
            } else {
                codeWriter->copyTerm(rexpr, stmt);
            }
            // Comma is seldom used, may be concatenation was intended
            ScDiag::reportScDiag(stmt->getBeginLoc(), ScDiag::SYNTH_CPP_COMMA);
            
        } else
        if (opcode == BO_Add || opcode == BO_Sub || opcode == BO_Mul || 
            opcode == BO_Div || opcode == BO_Rem || 
            opcode == BO_Shl || opcode == BO_Shr || 
            opcode == BO_And || opcode == BO_Or  || opcode == BO_Xor) 
        {
            if (lhsPointer || rhsPointer) {
                ScDiag::reportScDiag(stmt->getBeginLoc(), 
                                     ScDiag::SYNTH_POINTER_OPER) << opcodeStr;
            } else {
                if (opcode == BO_Shr) {
                    opcodeStr = ">>>";
                } else 
                if (opcode == BO_Shl) {
                    opcodeStr = "<<<";
                }
                
                // RHS operand is self-determined in Verilog (see LRM 11.6)
                if (codeWriter->isIncrWidth(rexpr) && 
                    (opcode == BO_Shr || opcode == BO_Shl)) {
                    // 32bit is always enough for shift value
                    size_t width = codeWriter->getExprTypeWidth(rexpr, 32);
                    width = (width > 32) ? 32 : width;
                    codeWriter->extendTypeWidth(rexpr, width);
                }
                
                // LHS operand is sized to result width in Verilog, 
                // so it needs to be updated for SHR/DIV/REM only
                if (codeWriter->isIncrWidth(lexpr) && 
                    (opcode == BO_Shr || opcode == BO_Div || opcode == BO_Rem)) {
                    // Try to get expression/type width
                    size_t width = codeWriter->getExprTypeWidth(lexpr);
                    codeWriter->extendTypeWidth(lexpr, width);
                }
                codeWriter->putBinary(stmt, opcodeStr, lexpr, rexpr);
            }

        } else 
        if (opcode == BO_GT || opcode == BO_LT || opcode == BO_GE || 
            opcode == BO_LE || opcode == BO_EQ || opcode == BO_NE) 
        {
            // Comparison allowed for pointers, but evaluated into constant
            if (lhsPointer || rhsPointer) {
                if (evaluateConstIntNoCheck(stmt, val)) {
                    codeWriter->putLiteral(stmt, SValue(SValue::boolToAPSInt(
                                           val.getBoolValue()), 10));
                } else {
                    ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                         ScDiag::SYNTH_UNSUPPORTED_OPER) << 
                                         (opcodeStr+" for pointer(s)") << 
                                         "ScGenerateExpr::parseBinaryStmt";
                }
            } else {
                codeWriter->putBinary(stmt, opcodeStr, lexpr, rexpr);
            }
            
        } else {
            ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                 ScDiag::SYNTH_UNSUPPORTED_OPER) 
                                 << opcodeStr << "ScGenerateExpr::parseBinaryStmt";
        }
    }
}

// Parse statement and run @chooseExprMethod for each operand
void ScGenerateExpr::parseCompoundAssignStmt(CompoundAssignOperator* stmt, 
                                             SValue& val) 
{
    Expr* lexpr = stmt->getLHS();
    Expr* rexpr = stmt->getRHS();

    // Parse left and right parts to fill @terms
    SValue lval; SValue rval;
    chooseExprMethod(lexpr, lval);
    chooseExprMethod(rexpr, rval);
    
    QualType ltype = lexpr->getType();
    QualType rtype = rexpr->getType();
    bool isPtr = isPointer(ltype) || isPointer(rtype);
    
    // Left value is result of assignment statement
    val = lval;

    // Operation code
    BinaryOperatorKind opcode = stmt->getOpcode();
    BinaryOperatorKind compOpcode = stmt->getOpForCompoundAssignment(opcode);
    string opcodeStr = stmt->getOpcodeStr(compOpcode);

    if (opcode == BO_AddAssign || opcode == BO_SubAssign || 
        opcode == BO_MulAssign || opcode == BO_DivAssign ||
        opcode == BO_RemAssign || opcode == BO_ShlAssign ||
        opcode == BO_ShrAssign || opcode == BO_AndAssign ||
        opcode == BO_OrAssign || opcode == BO_XorAssign) 
    {
        if (isPtr) {
            ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                 ScDiag::SYNTH_POINTER_OPER) << (opcodeStr+"=");
        } else {
            
            // RHS operand is self-determined in Verilog (see LRM 11.6)
            if (codeWriter->isIncrWidth(rexpr) && 
                (opcode == BO_ShrAssign || opcode == BO_ShlAssign)) {
                // 32bit is always enough for shift value
                size_t width = codeWriter->getExprTypeWidth(rexpr, 32);
                width = (width > 32) ? 32 : width;
                codeWriter->extendTypeWidth(rexpr, width);
            }
            
            // Put sign cast for LHS if RHS is signed
            if (!isSignedType(ltype) && isSignedType(rtype)) { 
                codeWriter->putSignCast(lexpr, CastSign::SCAST);
            }

            // Get owner variable, required for arrays
            SValue lvar = state->getVariableForValue(lval);

            codeWriter->putCompAssign(stmt, opcodeStr, lvar, lexpr, rexpr);
        }    
    } else {
        ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                             ScDiag::SYNTH_UNSUPPORTED_OPER) << opcodeStr
                             << "ScGenerateExpr::parseCompoundAssignStmt";
    }
}

// Parse statement and run @chooseExprMethod for the operand
void ScGenerateExpr::parseUnaryStmt(UnaryOperator* stmt, SValue& val) 
{
    Expr* rexpr = stmt->getSubExpr();
    // There is no value for general unary statement
    val = NO_VALUE;
    
    // Parse right part to fill @terms
    SValue rval;
    chooseExprMethod(rexpr, rval);
    
    auto rtype = rval.getTypePtr();
    bool isPointer = rtype && rtype->isPointerType();

    // Operation code
    UnaryOperatorKind opcode = stmt->getOpcode();
    string opcodeStr = stmt->getOpcodeStr(stmt->getOpcode());
    
    if (opcode == UO_Plus) {
        val = rval;
        if (isPointer) {
            // Pointer argument variable name, skip non-variable as array element
            if (val.isVariable() || val.isTmpVariable()) {
                // Try to get channel value from pointe
                SValue cval = getChannelFromState(rval);
                codeWriter->storePointerVarDecl(val, rval, cval);
                
            } else {
                codeWriter->copyTerm(rexpr, stmt);
            }
        } else {
            codeWriter->copyTerm(rexpr, stmt);
        }
    } else 
    if (opcode == UO_Minus)
    {
        if (isPointer) {
            ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                 ScDiag::SYNTH_POINTER_OPER) << opcodeStr;
        } else {
            // Literal considered inside of @putUnary
            codeWriter->putUnary(stmt, opcodeStr, rexpr, true);
        }

    } else
    if (opcode == UO_Not)
    {
        if (isBoolArgument(rexpr)) {
            ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                 ScDiag::CPP_BOOL_BITWISE_NOT);
        }
        
        if (isPointer) {
            ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                 ScDiag::SYNTH_POINTER_OPER) << opcodeStr;
        } else {
            codeWriter->putUnary(stmt, opcodeStr, rexpr, true);
        }

    } else
    if (opcode == UO_LNot) 
    {
        if (isPointer) {
            SValue ival;
            if (evaluateConstIntNoCheck(stmt, ival)) {
                codeWriter->putLiteral(stmt, SValue(SValue::boolToAPSInt(
                                       ival.getBoolValue()), 10));
                val = val;
                
            } else {
                ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                     ScDiag::SYNTH_UNSUPPORTED_OPER) << opcodeStr 
                                     << "ScGenerateExpr::parseUnaryStmt for pointer";
            }
        } else {
            codeWriter->putUnary(stmt, opcodeStr, rexpr, true);
        }

    } else    
    if (opcode == UO_PreInc || opcode == UO_PreDec)
    {
        if (isPointer) {
            ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                 ScDiag::SYNTH_POINTER_OPER) << opcodeStr;
        } else {
            codeWriter->putUnary(stmt, opcodeStr, rexpr, true);
        }

    } else
    if (opcode == UO_PostInc || opcode == UO_PostDec)
    {
        // Do not set @val to support conversion to boolean with brackets
        if (isPointer) {
            ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                 ScDiag::SYNTH_POINTER_OPER) << opcodeStr;
        } else {
            codeWriter->putUnary(stmt, opcodeStr, rexpr, false);
        }
        
    } else 
    if (opcode == UO_Deref) {
        // If @rval is reference it de-referenced in getValue()
        // Dereferenced value provided in @val
        state->getValue(rval, val);
        
        // Get variable for @rval
        SValue rvar = state->getVariableForValue(rval);

        if (rval.isArray()) {
            // Array cannot contains pointers to variables, so it is variable itself
            codeWriter->copyTerm(rexpr, stmt);
            
        } else 
        if (rval.isVariable() || rval.isTmpVariable()) {
            // Try to get integer value from constant variable or dynamic pointe
            if (val.isVariable() || val.isSimpleObject()) {
                // Check pointer and pointe both are constant type, 
                // for dynamic object constantness specified by pointer only
                bool isConst = isPointerToConst(rval.getType()) && 
                               (val.getType().isConstQualified() ||
                                val.isSimpleObject());
                
                if (isConst) {
                    SValue ival = state->getValue(val);
                    if (ival.isInteger()) {
                        codeWriter->putLiteral(stmt, ival);
                        return;
                    }
                }
            }
            
            if (val.isVariable()) {
                // Put de-referenced pointer variable 
                putPtrDerefExpr(stmt, rvar, rval, val);
                
            } else 
            if (val.isInteger()) {
                // Put pointer integer value, used for null pointer de-reference
                codeWriter->putLiteral(stmt, val);
                
            } else 
            if (val.isSimpleObject()) {
                // Put pointer variable, used for pointer to channel and dynamic object
                codeWriter->copyTerm(rexpr, stmt);
                
            } else
            if (val.isArray()) {
                // Array to pointer cast, put pointer variable as best option
                ScDiag::reportScDiag(stmt->getBeginLoc(), 
                                     ScDiag::SYNTH_ARRAY_TO_POINTER);
                codeWriter->copyTerm(rexpr, stmt);
                
            } else {
                // Dangling pointer de-references, put pointer variable as best option
                ScDiag::reportScDiag(stmt->getBeginLoc(), 
                                     ScDiag::CPP_DANGLING_PTR_DEREF);
                codeWriter->copyTerm(rexpr, stmt);
            }
            
        } else {
            // Put de-referenced pointer variable 
            putPtrDerefExpr(stmt, rvar, rval, val);
        }
            
    } else 
    if (opcode == UO_AddrOf) 
    {
        // "&" in process function not supported 
        ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                             ScDiag::SYNTH_UNSUPPORTED_OPER) << "&" 
                             << "ScGenerateExpr::parseUnaryStmt";
        
        // Get referenced variable for @rval
        SValue rrval;
        state->getDerefVariable(rval, rrval); rval = rrval;
        
        if (rval.isVariable() || rval.isTmpVariable() || rval.isArray() || 
            rval.isSimpleObject()) {
            // Create temporary object pointer to @rval
            QualType type = (rval.isVariable()) ? 
                                rval.getVariable().getType() :
                                ((rval.isTmpVariable()) ? 
                                        rval.getTmpVariable().getType() : 
                                        rval.getObjectPtr()->getType());
            val = SValue(type);
            // Put address of value to state, no code generated here
            state->putValue(val, rval);
            state->setValueLevel(val, level+1);
            
        } else {
            ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                 ScDiag::CPP_INCORRECT_REFER) << rval.asString();
        }
    } else {
        ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                             ScDiag::SYNTH_UNSUPPORTED_OPER) << opcodeStr << 
                             "ScGenerateExpr::parseUnaryStmt";
    }  
}

// Common function for operator[] in @ArraySubscriptExpr and @CXXOperatorCall
SValue ScGenerateExpr::parseArraySubscript(Expr* expr, 
                                           Expr* baseExpr, 
                                           Expr* indxExpr)
{
    // Result value
    SValue val = NO_VALUE;

    SValue bval;  // Array variable 
    chooseExprMethod(baseExpr, bval);
    
    // Disable part selection limitation to evaluate constants for index
    bool calcPartSelectArgOrig = calcPartSelectArg;
    calcPartSelectArg = false;
    
    // Skip sign cast for array index
    bool skipSignCastOrig = codeWriter->isSkipSignCast();
    codeWriter->setSkipSignCast(true);
    // Index variable/value
    SValue ival;    
    chooseExprMethod(indxExpr, ival);
    calcPartSelectArg = calcPartSelectArgOrig;
    codeWriter->setSkipSignCast(skipSignCastOrig);
    
    // Get referenced variable for array (reference to array: T (&a)[N])
    SValue bbval;  
    state->getDerefVariable(bval, bbval);
    //cout << "parseExpr(ArraySubscriptExpr) " << hex << expr << dec 
    //     << ", bval " << bval << ", bbval " << bbval << ", ival " << ival << endl;
    
    // Return value of array element, it can be sub-array
    if (bbval.isVariable() || bbval.isTmpVariable() || bbval.isArray()) {
        // Provide array element, no offset as only zero elements used
        state->getValue(bbval, val, false);
        
    } else {
        ScDiag::reportScDiag(expr->getBeginLoc(), ScDiag::SYNTH_NO_ARRAY) << bbval;
        // Required to prevent more errors
        SCT_INTERNAL_FATAL_NOLOC ("No array found");
    }
    
    // Get most inner array object and put it into @aval
    SValue aval(val); SValue rval(val);
    while (rval.isArray()) {
        aval = rval;
        state->getValue(aval, rval);
    }
    
    // Store subscript index to for record array
    codeWriter->addSubscriptIndex(aval, indxExpr);
    
    // Check if this operator is for port/signal
    QualType type = expr->getType();
    // Use @isPointer as sc_port<IF> cannot point-to channel
    bool isChannelPointer = isPointer(type) ? 
                            isScChannel(type->getPointeeType()) : false;
    
    // Put statement string to code writer
    if (isScChannel(type) || isScVector(type) || isChannelPointer) {
        // Get channel value
        SValue cval = getChannelFromState(val);

        if (cval.isScChannel()) {
            codeWriter->putArrayIndexExpr(expr, baseExpr, indxExpr);

        } else {
            ScDiag::reportScDiag(expr->getBeginLoc(), ScDiag::SYNTH_NO_CHANNEL);
        }
    } else {
        // Try to evaluate expression as integer constant
        SValue rval;
        if (evaluateConstInt(expr, rval, true)) {
            // Put literal instead of array element
            codeWriter->putValueExpr(expr, rval);
            
        } else {
            // Non-constant expression 
            codeWriter->putArrayIndexExpr(expr, baseExpr, indxExpr);
        }
    }
    return val;
}

// Array index access operator []
void ScGenerateExpr::parseExpr(ArraySubscriptExpr* expr, SValue& val)
{
    val = parseArraySubscript(expr, expr->getBase(), expr->getIdx());
}

// Function call expression
void ScGenerateExpr::parseCall(CallExpr* expr, SValue& val) 
{
    // There is no value for general function call
    val = NO_VALUE;
    
    // Get arguments
    Expr** args = expr->getArgs();
    unsigned argNum = expr->getNumArgs();

    // Get function name and type
    FunctionDecl* funcDecl = expr->getDirectCallee();
    if (funcDecl == nullptr) {
        SCT_INTERNAL_FATAL(expr->getBeginLoc(), 
                         "No function found for call expression");
    }
    
    string fname = funcDecl->getNameAsString();
    QualType retType = funcDecl->getReturnType();
    auto nsname = getNamespaceAsStr(funcDecl);
    
    if (DebugOptions::isEnabled(DebugComponent::doGenFuncCall)) {
        cout << "parseCall nsname : " << (nsname ? *nsname : "-- ")
             << ", fname : " << fname  << ", type : " << retType.getAsString() << endl;
    }
    
    // Check SC data type functions
    bool isScTypeFunc = isAnyScIntegerRef(retType);
    if (argNum > 0) {
        isScTypeFunc = isScTypeFunc || isAnyScIntegerRef(args[0]->getType());
    }            

    if (fname == "sct_assert") {
        // Checking assertion in regression tests
        SCT_TOOL_ASSERT (argNum == 1 || argNum == 2, "Incorrect argument number");
        
        std::string msgStr; 
        if (argNum == 2) {
            if (auto str = getStringFromArg(args[1])) {
                msgStr = str.getValue();
            }
        }

        SValue rval;
        chooseExprMethod(args[0], rval);
        
        if (!noSvaGenerate) {
            codeWriter->putAssert(expr, args[0], msgStr);
        }
        
    } else 
    if (fname == "sct_assert_const" || fname == "__assert" || 
        fname == "sct_assert_unknown") {
        // Do nothing, just remove sub-statements
        // Set @skipTerm to ignore (not write) these terms, required as
        // @rexpr term could absent in @terms that leads to error 
        codeWriter->setSkipTerm(true);
        SValue rval;
        chooseExprMethod(args[0], rval);
        codeWriter->setSkipTerm(false);
        
    } else 
    if (fname == "sct_assert_defined" || fname == "sct_assert_register" ||
        fname == "sct_assert_read" || fname == "sct_assert_latch") {
        // Do nothing, just remove sub-statements
        // Set @skipTerm to ignore (not write) these terms, required as
        // @rexpr term could absent in @terms that leads to error 
        codeWriter->setSkipTerm(true);
        SValue rval;
        chooseExprMethod(args[0], rval);
        codeWriter->setSkipTerm(false);
        
    } else 
    if (fname == "sct_assert_level") {
        // Do nothing, just remove sub-statements
        // Set @skipTerm to ignore (not write) these terms, required as
        // @rexpr term could absent in @terms that leads to error 
        codeWriter->setSkipTerm(true);
        SValue rval;
        chooseExprMethod(args[0], rval);
        codeWriter->setSkipTerm(false);
        
    } else 
    if (fname == "sct_assert_in_proc_start") {
        if (!noSvaGenerate) codeWriter->setParseSvaArg(true);
        
    } else 
    if (fname == "sct_assert_in_proc_func") {
        // Called in @SCT_ASSERT macro
        SCT_TOOL_ASSERT (argNum == 4 || argNum == 5, 
                         "Incorrect argument number");

        SValue fval; SValue sval;
        chooseExprMethod(args[0], fval);
        chooseExprMethod(args[1], fval);
        assertName = getStringFromArg(args[2]);
        evaluateConstIntNoCheck(args[3], fval);
        if (argNum == 5) {
            evaluateConstIntNoCheck(args[4], sval);
        } else {
            sval = fval;
        }

        if (!noSvaGenerate) {
            if (fval.isInteger() && sval.isInteger()) {
                std::string timeStr = parseSvaTime(
                                            fval.getInteger().getExtValue(), 
                                            sval.getInteger().getExtValue());
                codeWriter->putTemporalAssert(expr, args[0], args[1], timeStr);

            } else {
                ScDiag::reportScDiag(expr->getBeginLoc(), 
                                     ScDiag::SYNTH_SVA_INCORRECT_TIME);
            }
            codeWriter->setParseSvaArg(false);
        }
        
    } else 
    if (fname == "wait") {
        SCT_TOOL_ASSERT (nsname && *nsname == "sc_core",
                         "wait() in non-sc_core namespace");
                
        // Add nothing for wait() and counter assignment for wait(n)
        // Other logic implemented in ScTraverseProc
        if (argNum == 1) {
            // Parse @wait(n) argument
            //cout << "TYPE " << args[0]->getType().getAsString() << endl;
            
            // Check argument is integer type
            if (isAnyInteger(args[0]->getType())) {
                SValue ival;
                chooseExprMethod(args[0], ival);

                // Put wait(n) counter assignment
                codeWriter->putWaitNAssign(expr, args[0]);
            }
        }
    } else 
    if (nsname && *nsname == "sc_dt" && isScTypeFunc) {
        // SC data type functions
        if (fname == "concat") {
            SCT_TOOL_ASSERT (argNum == 2, "Incorrect argument number");
            Expr* lexpr = args[0];
            Expr* rexpr = args[1];
            
            // Parse argument expression to fill @terms
            SValue lval; SValue rval;
            chooseExprMethod(lexpr, lval);
            chooseExprMethod(rexpr, rval);
            
            // Report implicit integer to boolean cast, can be non-intended
            if (isIntToBoolCast(lexpr) || isIntToBoolCast(rexpr)) {
                ScDiag::reportScDiag(expr->getBeginLoc(), 
                                     ScDiag::SC_CONCAT_INT_TO_BOOL);
            }
            
            codeWriter->putConcat(expr, lexpr, rexpr);
            
        } else     
        if (fname == "and_reduce" || fname == "or_reduce" || 
            fname == "xor_reduce" || fname == "nand_reduce" || 
            fname == "nor_reduce" || fname == "xnor_reduce") {
            SCT_TOOL_ASSERT (argNum == 1, "Incorrect argument number");
            Expr* fexpr = args[0];
            
            string opcode = (fname == "and_reduce") ? "&" :
                            (fname == "or_reduce") ? "|" : 
                            (fname == "xor_reduce") ? "^" :
                            (fname == "nand_reduce") ? "~&" :
                            (fname == "nor_reduce") ? "~|" : "~^";
            
            // Parse argument expression to fill @terms
            SValue rval;
            chooseExprMethod(fexpr, rval);
            
            // Put reduction unary expression
            codeWriter->putUnary(expr, opcode, fexpr);
            
        } else {
            SCT_INTERNAL_FATAL(expr->getBeginLoc(), 
                               string("Unsupported function for SC type") + fname);
        }
    } else 
    if (nsname && *nsname == "sc_core" && 
        (fname == "report" || fname == "sc_time_stamp" || 
         fname == "sc_delta_count")) {
        
        // Do nothing, just remove sub-statements
        // Set @skipTerm to ignore (not write) these terms, required as
        // @rexpr term could absent in @terms that leads to error 
        codeWriter->setSkipTerm(true);
        for (unsigned i = 0; i < argNum; i++) {
            evalSubExpr(args[i]);
        }
        codeWriter->setSkipTerm(false);

    } else 
    if (((nsname && *nsname == "std") || isLinkageDecl(funcDecl)) &&
        (fname == "printf" || fname == "fprintf" || 
         fname == "sprintf" || fname == "snprintf" ||
         fname == "fopen" || fname == "fclose"))
    {
        // Do nothing
        
    } else 
    if (fname == "__assert_fail") {
        // This function has NORETURN attribute and cannot be supported
        //cout << "__assert_fail at " << expr->getSourceRange().getBegin().printToString(sm) << endl;
        ScDiag::reportScDiag(expr->getBeginLoc(), ScDiag::SC_ERROR_NORETURN);
        
    } else {
        // General functions, most logic is in ScTraverseProc 

        // Declare temporary return variable in current module
        if (!isVoidType(retType)) {
            SValue retVal(retType, modval);
            codeWriter->putVarDecl(nullptr, retVal, retType, nullptr);
            val = retVal;
            if (DebugOptions::isEnabled(DebugComponent::doGenFuncCall)) {
                cout << "Return value " << retVal.asString() << endl;
            }
        }
    }
}

// Member function call expression
void ScGenerateExpr::parseMemberCall(CXXMemberCallExpr* expr, SValue& val) 
{
    // There is no value for method call
    val = NO_VALUE;
    
    // Get arguments
    Expr** args = expr->getArgs();
    unsigned argNum = expr->getNumArgs();

    // Get method
    FunctionDecl* methodDecl = expr->getMethodDecl()->getAsFunction();
    string fname = methodDecl->getNameAsString();
    QualType retType = methodDecl->getReturnType();
    
    // Get @this expression and its type
    Expr* thisExpr = expr->getImplicitObjectArgument();
    QualType thisType = thisExpr->getType();
    // Method called for this pointer "->"
    bool isPointer = thisType->isAnyPointerType();
    bool isScIntegerType = isAnyScIntegerRef(thisType, true);
    
    // Do not replace constants with literal for part selection operator
    bool calcPartSelectArgOrig = calcPartSelectArg;
    calcPartSelectArg = isScIntegerType && 
                        (fname == "bit" || fname == "operator[]" ||
                         fname == "range" || fname == "operator()");
    
    // Get value for @this
    SValue tval;
    chooseExprMethod(thisExpr, tval);
    calcPartSelectArg = calcPartSelectArgOrig;
    
    if (DebugOptions::isEnabled(DebugComponent::doGenFuncCall)) {
        cout << "CXXMemberCallExpr this = " << thisType.getAsString() << ", fname = " 
             << fname  << ", return type = " << retType.getAsString() 
             << ", tval " << tval << ", module is " << modval << endl;
    }
    
    // Pointer de-reference
    if (isPointer) {
        SValue ttval;
        state->getValue(tval, ttval);
        tval = ttval;

        if (tval.isArray()) {
            // Array to pointer cast
            ScDiag::reportScDiag(expr->getBeginLoc(), 
                                 ScDiag::SYNTH_ARRAY_TO_POINTER);
        }
    }
    bool isScCoreObject = isAnyScCoreObject(thisType);
    
    // Parse method function call
    if (isScCoreObject && 
        (fname == "print" || fname == "dump" || fname == "kind")) {
        // Do nothing 
        
    } else 
    if (isScIntegerType) {
        // SC integer type object
        if (fname.find("operator") != string::npos && (
            fname.find("sc_unsigned") != string::npos ||
            fname.find("int") != string::npos ||
            fname.find("long") != string::npos ||  
            fname.find("bool") != string::npos)) {
            // Type conversion
            codeWriter->copyTerm(thisExpr, expr);

        } else 
        if (fname == "bit" || fname == "operator[]") {
            SCT_TOOL_ASSERT (argNum == 1, "Incorrect argument number");
            Expr* indxExpr = args[0];
            // Skip sign cast for part select index
            bool skipSignCastOrig = codeWriter->isSkipSignCast();
            codeWriter->setSkipSignCast(true);
            // Parse argument expression to fill @terms
            SValue rval;
            chooseExprMethod(indxExpr, rval);
            codeWriter->setSkipSignCast(skipSignCastOrig);
            
            codeWriter->putBitSelectExpr(expr, tval, thisExpr, indxExpr);            
            
        } else 
        if (fname == "range" || fname == "operator()") {
            SCT_TOOL_ASSERT (argNum == 2, "Incorrect argument number");
            Expr* hiExpr = args[0];
            Expr* loExpr = args[1];

            // Skip sign cast for part select index
            bool skipSignCastOrig = codeWriter->isSkipSignCast();
            codeWriter->setSkipSignCast(true);
            // Parse to remove sub-statements
            SValue rval;
            chooseExprMethod(hiExpr, rval);
            chooseExprMethod(loExpr, rval);

            // Evaluate range par-select which is possible variable dependent
            bool useDelta = evaluateRangeExpr(hiExpr, loExpr);
            codeWriter->setSkipSignCast(skipSignCastOrig);
            
            codeWriter->putPartSelectExpr(expr, tval, thisExpr, hiExpr, loExpr, useDelta);
            
        } else 
        if (fname == "and_reduce" || fname == "or_reduce" || 
            fname == "xor_reduce" || fname == "nand_reduce" || 
            fname == "nor_reduce" || fname == "xnor_reduce") {
            
            string opcode = (fname == "and_reduce") ? "&" :
                            (fname == "or_reduce") ? "|" : 
                            (fname == "xor_reduce") ? "^" :
                            (fname == "nand_reduce") ? "~&" :
                            (fname == "nor_reduce") ? "~|" : "~^";
            
            // Put reduction unary expression for @this
            codeWriter->putUnary(expr, opcode, thisExpr);    
            
        } else 
        if (fname.find("to_i") != string::npos ||
            fname.find("to_u") != string::npos ||
            fname.find("to_long") != string::npos) {
            // Type conversion
            QualType type = expr->getType();
            codeWriter->putTypeCast(thisExpr, expr, type);

            bool isSigned = isSignedType(type);
            bool isOrigSigned = isSignedType(thisType);

            // Set/reset sign cast for literals and expressions
            if (isOrigSigned != isSigned) { 
                codeWriter->putSignCast(expr, isSigned ? CastSign::SCAST : 
                                                         CastSign::UCAST);
            }
        } else
        if (fname.find("to_bool") != string::npos) {
            // No type cast required for to_bool as it applied for @bit_ref only
            codeWriter->copyTerm(thisExpr, expr);
            
        } else
        if (fname.find("operator") != string::npos) {
            // ->operator=, ++, --, +=, -=, ... not supported for now 
            // as it is not widely used form, it difficult to implement for arrays
            ScDiag::reportScDiag(expr->getSourceRange().getBegin(), 
                                 ScDiag::SYNTH_UNSUPPORTED_OPER) << fname 
                                 << "ScGenerateExpr::parseMemberCall";

        } else {
            ScDiag::reportScDiag(expr->getSourceRange().getBegin(), 
                                 ScDiag::SYNTH_UNSUPPORTED_OPER) << fname 
                                 << "ScGenerateExpr::parseMemberCall";
        }
        
    } else 
    if (isScChannel(thisType)) {
        // Channel object
        SValue cval = getChannelFromState(tval);
        
        if (fname == "write" && (cval.isScOutPort() || cval.isScSignal())) {
            // Output port write method
            SCT_TOOL_ASSERT (argNum == 1, "Incorrect argument number");
            Expr* argExpr = args[0];
            // Parse argument expression to fill @terms
            SValue rval;
            chooseExprMethod(argExpr, rval);
            
            // Put assignment instead of method call, use @cval as it is channel
            // No record possible here
            codeWriter->putAssign(expr, cval, thisExpr, argExpr);

        } else 
        if ((fname == "read" || (fname.find("operator") != string::npos && (
             fname.find("sc_int") != string::npos ||
             fname.find("sc_uint") != string::npos ||  
             fname.find("char") != string::npos ||
             fname.find("int") != string::npos ||
             fname.find("long") != string::npos ||
             fname.find("bool") != string::npos))) && cval.isScChannel()) {
            // Channel read access method
            SCT_TOOL_ASSERT (argNum == 0, "Incorrect argument number");

            codeWriter->copyTermRemoveBrackets(thisExpr, expr);
            // Return channel for read access, required to get channel for 
            // constant reference parameter to use channel variable
            val = cval;

        } else 
        if (fname.find("operator") != string::npos && (
            fname.find("sc_int") != string::npos ||
            fname.find("sc_uint") != string::npos ||  
            fname.find("char") != string::npos ||
            fname.find("int") != string::npos ||
            fname.find("long") != string::npos ||
            fname.find("bool") != string::npos)) {
            // Channel read/access operator
            
            // Copy terms for this expression
            codeWriter->copyTerm(thisExpr, expr);
            // Return channel for read access, required to get channel for 
            // constant reference parameter to use channel variable
            val = cval;

        } else 
        if (fname.find("pos") != string::npos || 
            fname.find("neg") != string::npos) {
            
            bool posEdge = fname.find("pos") != string::npos;
            bool negEdge = fname.find("neg") != string::npos;
            
            // Used for @SCT_PROPERTY
            codeWriter->putClockEdge(expr, thisExpr, posEdge, negEdge);
            
        } else {
            ScDiag::reportScDiag(expr->getSourceRange().getBegin(), 
                                 ScDiag::SYNTH_UNSUPP_CHANNEL_METH) << fname;
        }
    } else {
        // Non-channel/non-SCType method call
        if (isScCoreObject && fname == "wait") {
            // Add nothing for wait() and counter assignment for wait(n)
            // Other logic implemented in ScTraverseProc
            if (argNum == 1) {
                //cout << "TYPE " << args[0]->getType().getAsString() << endl;
                // Check argument is integer type
                if (isAnyInteger(args[0]->getType())) {
                    // Parse @wait(n) argument
                    SValue ival;
                    chooseExprMethod(args[0], ival);
                    
                    // Put wait(n) counter assignment
                    codeWriter->putWaitNAssign(expr, args[0]);
                }
            }
            
        } else 
        if (isScCoreObject && fname == "name" ) {
            // Do nothing for specific @sc_core functions

        } else {
            // General methods, most logic is in ScTraverseProc 
            
            // Declare temporary return variable in current module
            if (!isVoidType(retType)) {
                SValue retVal(retType, modval);
                codeWriter->putVarDecl(nullptr, retVal, retType, nullptr);
                val = retVal;
                //cout << "Return value " << retVal.asString() << endl;
            }
        }
    }
}

// Operator call expression
void ScGenerateExpr::parseOperatorCall(CXXOperatorCallExpr* expr, SValue& val) 
{
    // There is generally no value for operator call
    val = NO_VALUE;
    
    // Get arguments
    Expr** args = expr->getArgs();
    unsigned argNum = expr->getNumArgs();
    // Get first argument type, it can be this object
    SCT_TOOL_ASSERT (argNum > 0, "Operator without arguments");
    Expr* lexpr = args[0];
    QualType thisType = lexpr->getType();

    // Get operator name
    FunctionDecl* funcDecl = expr->getCalleeDecl()->getAsFunction();
    string fname = funcDecl->getNameAsString();
    auto nsname = getNamespaceAsStr(funcDecl);
    
    // Get opcode and opcode string
    OverloadedOperatorKind opcode = expr->getOperator();
    string opStr = getOperatorSpelling(opcode);
    
    if (DebugOptions::isEnabled(DebugComponent::doGenFuncCall)) {
        cout << "ScGeneratExpr::parseOperatorCall fname : " << fname 
             << ", type : " << thisType.getAsString() << endl;
    }
    
    // Check LHS is temporary expression materialized into in memory value
    // Required for @sc_biguint/@sc_bigint
    bool lhsIsTempExpr = isa<MaterializeTemporaryExpr>(lexpr);
    
    if (expr->isAssignmentOp() && opcode == OO_Equal) {
        // Assignment "operator=" for all types including SC data types
        // Assignments with add, subtract, ... processed below
        // Output port/signal write method
        SCT_TOOL_ASSERT (argNum == 2, "Incorrect argument number");

        if (lhsIsTempExpr) {
            ScDiag::reportScDiag(expr->getBeginLoc(), 
                                 ScDiag::SYNTH_TEMP_EXPR_ARG);
        }
        
        // Parse argument expression to fill @terms
        SValue lval;
        chooseExprMethod(lexpr, lval);
        val = lval;
        
        // Get record indices from @arraySubIndices, it is erased after use
        SValue lrec; state->getValue(lval, lrec);
        string lrecSuffix;
        if (lrec.isRecord()) {
            auto lrecvecs = getRecVector(lrec);
            lrecSuffix = codeWriter->getRecordIndxs(lrecvecs);
        }

        SValue rval;
        Expr* rexpr = args[1];
        chooseExprMethod(rexpr, rval);
        
        SValue rrec; state->getValue(rval, rrec);
        string rrecSuffix;
        if (rrec.isRecord()) {
            auto rrecvecs = getRecVector(rrec);
            rrecSuffix = codeWriter->getRecordIndxs(rrecvecs);
        }
        
        // Get owner variable, required for arrays
        SValue lvar = state->getVariableForValue(lval);
        // Try to get channel value, used as key in @varTraits 
        SValue cval = getChannelFromState(lvar);
        if (cval) {
            lvar = cval;
        }

        // Put assign for record and others
        bool isRecord = isUserDefinedClass(lval.getType(), true);

        if (isRecord) {
            codeWriter->putRecordAssign(expr, lvar, lrec, rval, rrec, 
                                        lrecSuffix, rrecSuffix);
        } else {
            codeWriter->putAssign(expr, lvar, lexpr, rexpr);
        }
        
    } else 
    if (isScPort(thisType) && opcode == OO_Arrow) {
        // Operator "->" for @sc_port<IF>
        
        // Get value for @this which points-to module/MIF object
        SValue lval = evalSubExpr(lexpr);
        // Dereferenced value provided in @val
        state->getValue(lval, val);
        // Get variable for @rval
        SValue tvar = state->getVariableForValue(lval);
        //cout << "Operator -> tval " << tval << " ttval " << val << " tvar " << tvar << endl;

        if (tvar.isArray()) {
            // Remain array element access, there is array of pointers
            codeWriter->copyTerm(lexpr, expr);
            SCT_INTERNAL_ERROR_NOLOC ("Array of sc_port<IF> not supported yet");
            
        } else 
        if (lval.isVariable() || lval.isTmpVariable()) {
            // Use variable to pointer variable, required for array of pointers
            codeWriter->copyTerm(lexpr, expr);
        } else {
            // Put de-referenced pointer variable 
            putPtrDerefExpr(expr, tvar, lval, val);
        }
        
    } else 
    if (isScVector(thisType) && opcode == OO_Subscript) {
        // sc_vector access at index
        Expr* rexpr = args[1];
        val = parseArraySubscript(expr, lexpr, rexpr);

    } else
    if (isIoStream(thisType)) {
        // Parse arguments to remove sub-statements 
        // Set @skipTerm to ignore (not write) these terms, required as
        // @rexpr term could absent in @terms that leads to error 
        codeWriter->setSkipTerm(true);
        for (unsigned i = 0; i < argNum; ++i) {
            SValue rval;
            chooseExprMethod(args[i], rval);
        }
        codeWriter->setSkipTerm(false);

        // Do nothing for @cout << and @cin >>
        
    } else 
    if (nsname && *nsname == "sc_dt") {
        // SC data type operators
        
        // Do not replace constants with literal for part selection operator
        bool calcPartSelectArgOrig = calcPartSelectArg;
        calcPartSelectArg = (opcode == OO_Subscript || opcode == OO_Call);

        SValue lval;
        chooseExprMethod(lexpr, lval);
        calcPartSelectArg = calcPartSelectArgOrig;
        
        if (opcode == OO_Subscript) { // "[]"
            SCT_TOOL_ASSERT (argNum == 2, "Incorrect argument number");
            Expr* rexpr = args[1];

            if (lhsIsTempExpr) {
                ScDiag::reportScDiag(expr->getBeginLoc(), 
                                     ScDiag::SYNTH_TEMP_EXPR_ARG);
            }
            
            // Skip sign cast for part select index
            bool skipSignCastOrig = codeWriter->isSkipSignCast();
            codeWriter->setSkipSignCast(true);
            SValue rval; 
            chooseExprMethod(rexpr, rval);
            codeWriter->setSkipSignCast(skipSignCastOrig);
            
            // Put bit access expression as array index access
            codeWriter->putBitSelectExpr(expr, lval, lexpr, rexpr);

        } else 
        if (opcode == OO_Call) {  // "()"
            SCT_TOOL_ASSERT (argNum == 3, "Incorrect argument number");
            if (lhsIsTempExpr) {
                ScDiag::reportScDiag(expr->getBeginLoc(), 
                                     ScDiag::SYNTH_TEMP_EXPR_ARG);
            }

            // Skip sign cast for part select index
            bool skipSignCastOrig = codeWriter->isSkipSignCast();
            codeWriter->setSkipSignCast(true);
            SValue rval; SValue rrval;
            chooseExprMethod(args[1], rval);
            chooseExprMethod(args[2], rrval);
            
            // Evaluate range par-select which is possible variable dependent
            bool useDelta = evaluateRangeExpr(args[1], args[2]);
            codeWriter->setSkipSignCast(skipSignCastOrig);

            codeWriter->putPartSelectExpr(expr, lval, lexpr, args[1], args[2], 
                                          useDelta);
            
        } else 
        // "," which is @concat() here    
        if (opcode == OO_Comma) { 
            SCT_TOOL_ASSERT (argNum == 2, "Incorrect argument number");
            Expr* rexpr = args[1];
            SValue rval; 
            chooseExprMethod(rexpr, rval);
            
            // Check incorrect type conversion to @bool for operand with 
            // not specified length
            auto checkCastToBool = [expr](const Expr* arg) {
                if (auto castExpr = dyn_cast<ImplicitCastExpr>(arg)) {
                    auto castKind = castExpr->getCastKind();
                    if (castKind == CK_IntegralToBoolean) {
                        if (isa<BinaryOperator>(castExpr->getSubExpr())) {
                            ScDiag::reportScDiag(expr->getBeginLoc(), 
                                                ScDiag::SYNTH_CONCAT_CAST_BOOL);
                        }
                    }
                }
            };
            checkCastToBool(lexpr);
            checkCastToBool(rexpr);
            
            // Report implicit integer to boolean cast, can be non-intended
            if (isIntToBoolCast(lexpr) || isIntToBoolCast(rexpr)) {
                ScDiag::reportScDiag(expr->getBeginLoc(), 
                                     ScDiag::SC_CONCAT_INT_TO_BOOL);
            }
            
            codeWriter->putConcat(expr, lexpr, rexpr);

        } else 
        // "!" "~" 
        if (opcode == OO_Exclaim || opcode == OO_Tilde) {
            // Postfix ==/-- has artifical argument
            SCT_TOOL_ASSERT (argNum == 1, "Incorrect argument number");
            
            codeWriter->putUnary(expr, opStr, lexpr);
            
        } else 
        // "++" "--"
        if (opcode == OO_PlusPlus || opcode == OO_MinusMinus) {
            // Postfix ++/-- has artifical argument
            SCT_TOOL_ASSERT (argNum == 1 || argNum == 2, "Incorrect argument number");
            
            if (lhsIsTempExpr) {
                ScDiag::reportScDiag(expr->getBeginLoc(), 
                                     ScDiag::SYNTH_TEMP_EXPR_ARG);
            }
            
            SValue rval; 
            if (argNum == 2) chooseExprMethod(args[1], rval);
            
            codeWriter->putUnary(expr, opStr, lexpr, argNum == 1);
            
        } else    
        // Unary "-" and "+"
        if (argNum == 1 && (opcode == OO_Plus || opcode == OO_Minus)) 
        {
            if (opcode == OO_Minus) {
                codeWriter->putUnary(expr, opStr, lexpr, true);
                
            } else {
                val = lval;
                codeWriter->copyTerm(lexpr, expr);
            }
            
        } else   
        // "+" "-" "*"  "/" "==" "!=" "<" "<=" ">" ">=" "<<" ">>" "%" "^" "&" "|" 
        // There is no operators "&&" "||" for SC data types
        if (argNum == 2 && (opcode == OO_Plus || opcode == OO_Minus || 
            opcode == OO_Star || opcode == OO_Slash || opcode == OO_EqualEqual || 
            opcode == OO_ExclaimEqual || opcode == OO_Less ||
            opcode == OO_LessEqual || opcode == OO_Greater || 
            opcode == OO_GreaterEqual || opcode == OO_LessLess || 
            opcode == OO_GreaterGreater || opcode == OO_Percent || 
            opcode == OO_Caret || opcode == OO_Amp || opcode == OO_Pipe || 
            opcode == OO_AmpAmp || opcode == OO_PipePipe))
        {
            // As no &&/||, so no LHS based evaluation required
            SCT_TOOL_ASSERT (opcode != OO_AmpAmp && opcode != OO_PipePipe,
                             "No &&/|| for SC data types supported");
                    
            Expr* rexpr = args[1];
            SValue rval; 
            chooseExprMethod(rexpr, rval);
            
            if (opcode == OO_GreaterGreater) {
                opStr = ">>>";
            } else 
            if (opcode == OO_LessLess) {
                opStr = "<<<";        
            }

            // RHS operand is self-determined in Verilog (see LRM 11.6)
            // 32bit is always enough for shift value
            if (codeWriter->isIncrWidth(rexpr) && 
                (opcode == OO_GreaterGreater || opcode == OO_LessLess)) {
                // 32bit is always enough for shift value
                size_t width = codeWriter->getExprTypeWidth(rexpr, 32);
                width = (width > 32) ? 32 : width;
                codeWriter->extendTypeWidth(rexpr, width);
            }

            // LHS operand is sized to result width in Verilog, 
            // so it needs to be updated for SHR/DIV/REM only
            if (codeWriter->isIncrWidth(lexpr) && 
                (opcode == OO_Slash || opcode == OO_GreaterGreater || 
                 opcode == OO_Percent)) {
                // Try to get expression/type width
                size_t width = codeWriter->getExprTypeWidth(lexpr);
                codeWriter->extendTypeWidth(lexpr, width);
            }
            codeWriter->putBinary(expr, opStr, lexpr, rexpr);
            
        } else     
        // "+=" "-=" "*="  "/=" "%=" ">>=" "<<=" "&=" "|=" "^="
        if (opcode == OO_PlusEqual || opcode == OO_MinusEqual || 
            opcode == OO_StarEqual || opcode == OO_SlashEqual ||
            opcode == OO_PercentEqual || 
            opcode == OO_GreaterGreaterEqual || opcode == OO_LessLessEqual ||
            opcode == OO_AmpEqual || opcode == OO_PipeEqual || 
            opcode == OO_CaretEqual)
        {
            SCT_TOOL_ASSERT (argNum == 2, "Incorrect argument number");
            
            // Left value is result of assignment statement
            val = lval;
            
            if (lhsIsTempExpr) {
                ScDiag::reportScDiag(expr->getBeginLoc(), 
                                     ScDiag::SYNTH_TEMP_EXPR_ARG);
            }
            
            Expr* rexpr = args[1];
            SValue rval; 
            chooseExprMethod(rexpr, rval);

            if (opcode == OO_GreaterGreaterEqual) {
                opStr = ">>>";
            } else
            if (opcode == OO_LessLessEqual) {
                opStr = "<<<";
            } else {
                // Remove last symbol "=" from @opcode
                opStr = opStr.substr(0, opStr.length()-1);
            }
            
            // RHS operand is self-determined in Verilog (see LRM 11.6)
            // 32bit is always enough for shift value
            if (codeWriter->isIncrWidth(rexpr) && 
                (opcode == OO_GreaterGreaterEqual || opcode == OO_LessLessEqual)) {
                // 32bit is always enough for shift value
                size_t width = codeWriter->getExprTypeWidth(rexpr, 32);
                width = (width > 32) ? 32 : width;
                codeWriter->extendTypeWidth(rexpr, width);
            }
            
            // Get owner variable, required for arrays
            SValue lvar = state->getVariableForValue(lval);
            
            codeWriter->putCompAssign(expr, opStr, lvar, lexpr, rexpr);

        } else { 
            ScDiag::reportScDiag(expr->getSourceRange().getBegin(), 
                                 ScDiag::SYNTH_UNSUPPORTED_OPER) << opStr << 
                                 "ScGenerateExpr::parseOperatorCall";
        }
    } else {
        SCT_INTERNAL_FATAL(expr->getBeginLoc(), 
                           string("Unsupported operator ") + fname);
    }
}

// Return statement
void ScGenerateExpr::parseReturnStmt(ReturnStmt* stmt, SValue& val) 
{
    // There is generally no value for operator call
    val = NO_VALUE;

    Expr* expr = stmt->getRetValue();
    
    if (expr != nullptr) {
        // Remove @ExprWithCleanups from @CXXConstructExpr
        auto retExpr = removeExprCleanups(expr);
        // Check for copy constructor to use record RValue
        auto ctorExpr = getCXXCtorExprArg(retExpr);
        if (ctorExpr) {
            recCopyCtor = isCXXCopyCtor(ctorExpr);
            
            if (!recCopyCtor) {
                ScDiag::reportScDiag(expr->getBeginLoc(), 
                         ScDiag::SYNTH_NONTRIVIAL_COPY);
            }
       }

        // Set return temporal variable value to analyze @CXXConstructExpr and
        // use as record copy local variable
        locrecvar = returnValue; 

        // Parse return expression
        chooseExprMethod(expr, val);
        
        // Clear after
        locrecvar = NO_VALUE;
        
        // Store record value in return variable
        if (returnValue.isTmpVariable()) {
            assignValueInState(returnValue, val);
        } else
        if (returnValue.isUnknown()) {
            // Do nothing
        } else {
            SCT_TOOL_ASSERT (false, "Unexpected kind of return variable value");
        }
        
        // Assign to specified temporary variable
        // Put assign for record and others
        bool isRecord = isUserDefinedClass(returnValue.getType(), true);

        if (isRecord) {
            // Get field initialization string from @CXXConstructExpr
            if (ctorExpr) {
                codeWriter->copyTerm(ctorExpr, stmt);
            }
            
        } else {
            codeWriter->putAssign(stmt, returnValue, expr);
        }
        
        //cout << codeWriter->getStmtString(stmt).getValue() << endl;
    } else {
        // Nothing to write
    }
}

// Ternary operator (...) ? ... : ...
void ScGenerateExpr::parseConditionalStmt(ConditionalOperator* stmt, SValue& val) 
{
    // There is no value for unary statement yet
    val = NO_VALUE;
    
    Expr* cexpr = stmt->getCond();
    Expr* lexpr = stmt->getLHS();
    Expr* rexpr = stmt->getRHS();
    SValue cval; SValue fcval;
    // Parse condition to remove sub-statements
    chooseExprMethod(cexpr, cval);
    // Get condition for term
    getTermCondValue(stmt, cval, fcval);
    //cout << "? lexpr " << hex << stmt << dec << " cval " << cval << endl;
    
    // Try to calculate condition into integer constant
    if (cval.isInteger()) {
        if (cval.getInteger().isNullValue()) {
            SValue rval;
            chooseExprMethod(rexpr, rval);
            val = rval;
            codeWriter->copyTerm(rexpr, stmt);
            
        } else {
            SValue lval;
            chooseExprMethod(lexpr, lval);
            val = lval;
            codeWriter->copyTerm(lexpr, stmt);
        }
    } else {
        SValue lval; SValue rval;
        chooseExprMethod(lexpr, lval);
        chooseExprMethod(rexpr, rval);
        
        codeWriter->putCondStmt(stmt, cexpr, lexpr, rexpr);
    }
}

// Choose and run DFS step in accordance with expression type.
// Remove sub-statements from generator
void ScGenerateExpr::chooseExprMethod(Stmt *stmt, SValue &val)
{
    ScParseExpr::chooseExprMethod(stmt, val);
}

// ----------------------------------------------------------------------------
// Parse general statement 
llvm::Optional<string> ScGenerateExpr::parse(const Stmt* stmt)
{
    codeWriter->startStmt();
    SValue val;
    auto ncstmt = const_cast<Stmt*>(stmt);
    chooseExprMethod(ncstmt, val);
    return codeWriter->getStmtString(ncstmt);
}

// Parse block terminator statement 
llvm::Optional<string> ScGenerateExpr::parseTerm(const Stmt* stmt, 
                                                 const SValue& termCond, 
                                                 bool artifIf)
{
    codeWriter->startStmt();
    
    // It is prohibited to use any statement except IF in method w/o sensitivity
    if (codeWriter->isEmptySensitivity() && !isa<const IfStmt>(stmt) && 
        !isa<const ConditionalOperator>(stmt)) {
        ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                             ScDiag::SYNTH_NOSENSITIV_METH);
    }

    if (auto ifstmt = dyn_cast<const IfStmt>(stmt)) {
        auto cexpr = const_cast<Expr*>(ifstmt->getCond());
        SValue val;
        chooseExprMethod(cexpr, val);
        if (termCond.isInteger()) {
            codeWriter->putLiteral(cexpr, termCond);
            
        } else {
            // IF statement in method w/o sensitivity must be evaluated 
            // and removed, no Verilog code generated
            if (codeWriter->isEmptySensitivity()) {
                ScDiag::reportScDiag(stmt->getSourceRange().getBegin(), 
                                     ScDiag::SYNTH_NOSENSITIV_METH);
            }
        }
        
        return codeWriter->getIfString(cexpr);
        
    } else 
    if (auto swstmt = dyn_cast<const SwitchStmt>(stmt)) {
        auto cexpr = const_cast<Expr*>(swstmt->getCond());
        SValue val;
        chooseExprMethod(cexpr, val);
        if (termCond.isInteger()) {
            codeWriter->putLiteral(cexpr, termCond);
        }
        return codeWriter->getSwitchString(cexpr);

    } else  
    if (auto binstmt = dyn_cast<const BinaryOperator>(stmt)) {
        auto cexpr = binstmt->getLHS();
        SValue val;
        chooseExprMethod(cexpr, val);
        if (termCond.isInteger()) {
            codeWriter->putLiteral(cexpr, termCond);
        }
        return codeWriter->getStmtString(cexpr);    // Not used

    } else 
    if (auto cndstmt = dyn_cast<const ConditionalOperator>(stmt)) {
        auto cexpr = cndstmt->getCond();
        SValue val;
        chooseExprMethod(cexpr, val);
        if (termCond.isInteger()) {
            codeWriter->putLiteral(cexpr, termCond);
        }
        return codeWriter->getStmtString(cexpr);    // Not used

    } else 
    if (auto forstmt = dyn_cast<const ForStmt>(stmt)) {
        auto init  = const_cast<Stmt*>(forstmt->getInit());
        auto cexpr = const_cast<Expr*>(forstmt->getCond());
        auto incr  = const_cast<Expr*>(forstmt->getInc());
        SValue val;
        
        if (artifIf) {
            if (cexpr) {
                chooseExprMethod(cexpr, val);
            }
            return codeWriter->getIfString(cexpr);
            
        } else {
            if (init) {
                DeclStmt* declStmt = dyn_cast<DeclStmt>(init);
                if (declStmt && !declStmt->isSingleDecl()) {
                    ScDiag::reportScDiag(init->getSourceRange().getBegin(), 
                                         ScDiag::CPP_LOOP_COMPLEX_INIT);
                }
                // Set FOR loop mode to put declaration into @init statement string
                // If there is not declaration it means normal local variable
                // is used as loop counter
                codeWriter->setForLoopInit(declStmt);
                chooseExprMethod(init, val);
                codeWriter->setForLoopInit(false);
            }
            if (cexpr) {
                chooseExprMethod(cexpr, val);
                if (termCond.isInteger()) {
                    codeWriter->putLiteral(cexpr, termCond);
                }
            }
            if (incr) {
                BinaryOperator* binStmt = dyn_cast<BinaryOperator>(incr);
                if (binStmt && 
                    binStmt->getOpcodeStr(binStmt->getOpcode()) == ",") {
                    ScDiag::reportScDiag(incr->getSourceRange().getBegin(), 
                                         ScDiag::CPP_LOOP_COMPLEX_INCR);
                }
                chooseExprMethod(incr, val);
            }
            return codeWriter->getForString(init, cexpr, incr);
        }
        
    } else 
    if (auto whiStmt = dyn_cast<const WhileStmt>(stmt)) {
        auto cexpr = const_cast<Expr*>(whiStmt->getCond());
        SValue val;
        chooseExprMethod(cexpr, val);
        if (termCond.isInteger()) {
            codeWriter->putLiteral(cexpr, termCond);
        }
        
        if (artifIf) {
            return codeWriter->getIfString(cexpr);
        } else {
            return codeWriter->getWhileString(cexpr);
        }

    } else 
    if (auto doStmt = dyn_cast<const DoStmt>(stmt)) {
        auto cexpr = const_cast<Expr*>(doStmt->getCond());
        SValue val;
        chooseExprMethod(cexpr, val);
        if (termCond.isInteger()) {
            codeWriter->putLiteral(cexpr, termCond);
        }
        
        if (artifIf) {
            return codeWriter->getIfString(cexpr);
        } else {
            return codeWriter->getWhileString(cexpr);
        }

    } else 
    if (isa<const BreakStmt>(stmt)) {
        return codeWriter->getBreakString();

    } else 
    if (isa<const ContinueStmt>(stmt)) {
        return codeWriter->getContinueString();

    } else {
        SCT_INTERNAL_FATAL(stmt->getBeginLoc(), string("Unsupported terminator ")+
                         stmt->getStmtClassName());
        
        return llvm::Optional<string>();
    }    
}

}