# Tetris
[![Build Tetris Project](https://github.com/hui1601/2025-datastructure-tetris/actions/workflows/build.yml/badge.svg)](https://github.com/hui1601/2025-datastructure-tetris/actions/workflows/build.yml)

2025학년도 1학기 자료구조 Term Project

20243095 동헌희
## 테스트 환경
이 프로젝트는 MSYS2와 Ubuntu 24.04에서 테스트되었습니다.

MSYS2 환경에 대한 자세한 내용은 [MSYS2 공식 문서](https://www.msys2.org/docs/environments/)를 참고하세요.
### 로컬 테스트 환경
기본적인 툴체인이 설치된 환경에서 테스트를 진행했습니다.

| OS | Platform | Toolchain | C Library | Version |
|----|----------|-----------|-----------|---------|
| Windows 11 24H2 | MSYS2 MSYS | GCC | cygwin | 14.2.0 |
| Windows 11 24H2 | MSYS2 UCRT64 | GCC | ucrt | 14.2.0 |
| Windows 11 24H2 | MSYS2 MINGW64 | GCC | msvcrt | 14.2.0 |
| Windows 11 24H2 | MSYS2 CLANG64 | LLVM | ucrt | 19.1.7 |
| Ubuntu 24.04 | N/A | GCC | glibc | 14.2.0 |
| Ubuntu 24.04 | N/A | LLVM | libstdc++ | 19.1.7 |

### GitHub Actions
GitHub Actions를 사용하여 매 push 및 PR에 대해 자동으로 빌드 및 아티팩트를 생성합니다.

다음 환경에서 테스트됩니다.

| OS | Platform | Toolchain |
|----|----------|-----------|
| ubuntu-latest | N/A | GCC |
| windows-latest | MSYS2 MINGW64 | GCC |
| macos-latest | N/A | LLVM |

