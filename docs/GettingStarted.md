# Getting Started with Dynamatic++

## Overview

Welcome to the Dynamatic++ project!

This project is the natural successor of [Dynamatic](https://github.com/lana555/dynamatic) (often referred to as *legacy Dynamatic* throughout this repository), an academic, open-source high-level synthesis compiler based on LLVM IR that produces synthesizable synchronous dynamically-scheduled circuits (*elastic circuits*) from C/C++ code. Dynamatic enabled [numerous scientific publications](https://dynamatic.epfl.ch/) in top conferences over the years and is actively being used by many researchers in various research groups. Dynamatic++ aims to achieve the same goals (and more!) but uses the more recent and powerful MLIR ecosystem instead of LLVM IR. Furthermore, this project puts a lot more emphasis on code quality, reliability, and interoperability, while remaining as user-friendly as possible.

## Building the project

We use the [CMake](https://cmake.org/) build system to build and test Dynamatic++. You can find detailed instructions on how to build the project, including its software dependencies, in the `README.md` file at the top level of the repository.

# Software architecture

This section provides an overview of the software architecture of the project and is meant as an entry-point for users who would like to start digging into the codebase. It describes the project's directory structure, our software dependencies (i.e., git submodules), and our testing infrastructure.

## Directory structure

This section is intended to give an overview of the project's directory structure and an idea of what each directory contains to help new users more easily look for and find specific parts of the implementation. Note that the superproject is structured very similarly to CIRCT, which itself is structured very similarly to LLVM/MLIR, thus this overview is useful for navigating these repositories as well. For exploring/editing the codebase, we strongly encourage the use of an IDE with a *go to reference/implementation* feature (e.g., *VSCode*) to easily navigate between header/source files. Below is a visual representation of a subset of the project's directory structure, with basic information on what each directory contains.

```sh
├── bin # Symbolic links to commonly used binaries after build (untracked)
├── build # Files generated during build (untracked)
│   ├── bin # Binaries generated by the superproject
│       ├── include
│           ├── dynamatic # Compiled TableGen headers (*.h.inc)
├── circt # CIRCT repository (submodule)
├── docs # Documentation and tutorials, where this file lies
├── experimental # Experimental passes and tools
├── include
│   ├── dynamatic # All header files (*.h)
├── lib # Implementation of compiler passes (*.cpp)
│   ├── Conversion # Implementation of conversion passes (*.cpp)
│   ├── Transforms # Implementation of transform passes (*.cpp)
├── polygeist # Polygeist repository (submodule)
├── test # Unit tests
├── tools # Implementation of executables generated during build
│   ├── dynamatic-opt # Dynamatic++ optimizer
├── build.sh # Build script to build the entire project
└── CMakeLists.txt # Top level CMake file for building the superproject
```

## Dependencies

Dynamatic++ uses [git submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) to manage its two core software dependencies (both hosted on GitHub and described in more details in the next sections of t his document), [Polygeist](https://github.com/llvm/Polygeist) and [CIRCT](https://github.com/EPFL-LAP/circt). These two subprojects themselves depend on [LLVM/MLIR](https://github.com/llvm/llvm-project), which they also manage using a git submodule. This makes Dynamatic++ depend on, in practice, two different versions of LLVM/MLIR through nested submodules, the one used by CIRCT and the one used by Polygeist. The version of LLVM/MLIR that CIRCT depends on is considered the authoritative one in case of conflicting implementation since it's the one that is more consistently bumped and thus up-to-date. The project is set up so that, when you include LLVM/MLIR headers directly from Dynamatic++ code, it includes those that come from the LLVM/MLIR version that CIRCT depends on.

### Polygeist

[Polygeist](https://github.com/llvm/Polygeist) is a C/C++ frontend for MLIR including polyhedral optimizations and parallel optimizations features. Polygeist is thus responsible for the first step of our compilation process, that is taking source code written in C/C++ into the MLIR ecosystem. In particular, we care that our entry point to MLIR is at a very high semantic level, namely, at a level where polyhedral analysis is possible. The latter allows us to easily identify dependencies between memory accesses in source programs in a very accurate manner, which is key to optimizing the allocation of memory interfaces and resources in our elastic circuits down the line. Polygeist is able to emit MLIR code in the [*Affine*](https://mlir.llvm.org/docs/Dialects/Affine/) dialect, which is perfectly suited for this kind of analysis. 

### CIRCT

[CIRCT](https://github.com/EPFL-LAP/circt) is *"an (experimental!) effort looking to apply MLIR and the LLVM development methodology to the domain of hardware design tools"*. It is a very actively developed project from which we only use a small portion of the codebase (though we inherit a lot from their robust development methodologies and code infrastructure), namely, the *Handshake* dialect and the compiler passes/tools around it. *Handshake* allows us to model our elastic circuits (achieving the same goal as what `.dot` files written in the [DOT language](https://graphviz.org/doc/info/lang.html) achieve in legacy Dynamatic) within MLIR, letting us leverage the full power of the compiler infrastructure to optimize electronic circuits before emitting them to a synthesizable RTL design. We actually depend on a fork of CIRCT that we maintain and update regularly. This fork includes modifications and additions to the *Handshake* dialect required to represent *our* elastic circuits, which follow slightly different conventions than the ones from CIRCT's elastic circuits.  

### Working with submodules

Having a project with submodules means that you have to pay attention to a couple additional things when pulling/pushing code to the project to maintain it in sync with the submodules. If you are unfamiliar with submodules, you can learn more about how to work with them [here](https://git-scm.com/book/en/v2/Git-Tools-Submodules). Below is a very short and incomplete description of how our submodules are managed by our repository as well as a few pointers on how to perform simple git-related tasks in this context.

Along the history of Dynamatic++'s (in this context, called the *superproject*) directory structure and file contents, the repository stores the commit hash of a specific commit for each submodule's repository to identify the version of each *subproject* that the superproject currently depends on. These commit hashes are added and commited the same way as any other modification to the repository, and can thus evolve as development moves forward, allowing us to use more recent version of our submodules as they are pushed to their respective repositories. Here are a few concrete things you need to keep in mind while using the repository that may differ from your usual submodule-free workflow. 
- Clone the repository with `git clone --recurse-submodules git@github.com:EPFL-LAP/dynamatic.git` to instruct git to also pull and check out the version of the submodules referenced in the latest commit of Dynamatic++'s `main` branch.
- When pulling the latest commit(s), use `git pull --recurse-submodules` from the top level repository to also update the checked out commit from submodules in case the superproject changed the subprojects commits it is tracking.
- To commit changes made to files within CIRCT from the superproject (which is possible thanks to the fact that we use a fork of CIRCT), you first need to commit these changes to the CIRCT fork, and then update the CIRCT commit tracked by the superproject. More precisely,
  1. `cd` to the `circt` subdirectory,
  2. `git add` your changes and `git commit` them to the CIRCT fork,
  3. `cd` back to the top level directory,
  4. `git add circt` to tell the superproject to track your new CIRCT commit and `git commit` to Dynamatic++.
  
  If you want to push these changes to remote, note that you will need to `git push` *twice*, once from the `circt` subdirectory (the CIRCT commit) and once from the top level directory (the Dynamatic++ commit). 

## Testing

At this point, Dynamatic++ only features unit tests (integration tests to come in the future!) that evaluate the behavior of a small part of the implementation (typically, one compiler pass) against an expected output. All files within the `test` directory with the `.mlir` extension are automatically considered as unit test files. They can be ran/checked all at once by running `ninja check-dynamatic` from a terminal within the top level `build` directory. We use the [`FileCheck`](https://llvm.org/docs/CommandGuide/FileCheck.html) LLVM utility to compare the actual output of the implementation with the expected one. [`docs/Testing.md`](Testing.md) describes the [structure of `FileCheck` unit test files](Testing.md#understanding-filecheck-unit-test-files) and explains [how to create your own unit tests](Testing.md#creating-your-own-unit-tests-with-filecheck).

# Contributing 

**TODO**

## Issues & Pull requests

## Experimental work

## GitHub CI/CD
