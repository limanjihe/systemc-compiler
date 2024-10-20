# Intel&reg; Compiler for SystemC* 

*\*Other names and brands may be claimed as the property of others.*

## Introduction

Intel&reg; Compiler for SystemC* (ICSC) translates synthesizable SystemC design into equvivalent SystemVerilog code.

ICSC checks a SystemC design for common coding mistakes and generates human-readable SystemVerilog code. The tool supports SystemC synthesizable subset in method and thread processes, and arbitrary C++ code in module constructors. ICSC is based on Clang/LLVM 15.0.7 and includes SystemC 3.0.0.

See more information at [Intel Compiler for SystemC wiki](https://github.com/intel/systemc-compiler/wiki).

## Common SystemC Library

Common SystemC Library consists of types, modules and functions which could be used in SystemC designs and testbench code. The main part of the library are communication channels including Target/Initiator, FIFO, Register and others. The channels have functional interfaces similar to TLM 1.0.

There are [Communication channels training slides](https://github.com/intel/systemc-compiler/blob/main/doc/papers/single_source_training.pdf).

See more information at [Common SystemC Library](https://github.com/intel/systemc-compiler/wiki/Common-SystemC-Library) .

## License

ICSC is distributed under the [Apache License v2.0 with LLVM Exceptions](https://github.com/intel/systemc-compiler/blob/main/LICENSE.txt).

## Getting started

ICSC is based on Clang/LLVM frontend and can be installed at most Linux OS. There is ```install.sh``` script that downloads and builds ICSC and the required dependencies at **SLES12**, **Ubuntu 22.04**, and **Ubuntu 20.04**.

An instruction how to install and run ISCS is given at [Getting started](https://github.com/intel/systemc-compiler/wiki/Getting-started). 

## Documentation 

[User guide](https://github.com/intel/systemc-compiler/blob/main/doc/ug.pdf) document describes installation procedure, run tool options, preparation of SystemC design for synthesis, tool extensions and advanced verification features.

ICSC supports [SystemC Synthesizable Subset](https://www.accellera.org/images/downloads/drafts-review/SystemC_Synthesis_Subset_Draft_1_4.pdf). Details of SystemC/C++ subset supported are described at [SystemC/C++ supported](https://github.com/intel/systemc-compiler/wiki/SystemC--supported).

## Publications

* [Single Source library for high-level modelling and hardware synthesis](https://easychair.org/my/preprint_download_pdf?version=45879), at DvCon Europe 2024
* [Intel Compiler for SystemC and SystemC common library](https://github.com/intel/systemc-compiler/blob/main/doc/papers/common_library_2022.pdf) at CHIPS tech summit 2022
* [Temporal assertions in SystemC](https://github.com/intel/systemc-compiler/blob/main/doc/papers/sct_assert.pdf) at DvCon'2020 and SystemC evolution day'2020
* [SystemC-to-Verilog Compiler: a productivity-focused tool for hardware design in cycle-accurate SystemC](https://github.com/intel/systemc-compiler/blob/main/doc/papers/icsc.pdf) at DvCon'2019

## Help

To get help please [submit your question or issue](https://github.com/intel/systemc-compiler/issues)
