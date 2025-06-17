# Tetris
[![Build Tetris Project](https://github.com/hui1601/2025-datastructure-tetris/actions/workflows/build.yml/badge.svg)](https://github.com/hui1601/2025-datastructure-tetris/actions/workflows/build.yml)

2025학년도 1학기 자료구조 Term Project

20243095 동헌희

## 구현 설명
### 주요 기능
#### 게임 플레이
* 블록 생성 및 회전
* 블록 이동 및 고정
* 블록 충돌 감지
* 블록 Hold 기능
* Ghost 기능(수직 drop 시 미리보기 블록)
* 줄 제거 및 점수 계산(`lines_cleared * 100 + (lines_cleared * lines_cleared * 50)`)
* 7-bag 규칙 적용
* 점수에 따른 게임 속도 조절(`1 - (point / 1000000.0)` / MIN: 50ms)
* 게임 오버 시 1위면 축하 메시지 출력
#### 데이터 관리
* 자동으로 게임 데이터 저장 및 불러오기
* 점수를 Key로 하는 AVL 트리 구조 사용
* 게임 데이터 구조는 바이너리 형식으로 저장(Null-terminated 문자열+ 점수(uint64_t)+플레이 시간(time_t)으로 구성)
#### 렌더링
* 콘솔 기반 렌더링
* 멀티 플랫폼 지원(Windows, Linux, macOS)
* 렌더링 옵션 선택 가능(최초 실행 시 / tetris.dat 파일 삭제 후 재설정 가능)
  1. 블록문자 렌더링(■ 문자와 터미널 글자 색 변경으로 블록을 표시)
  2. 배경색 렌더링(공백만 사용하여 배경색을 변경하여 블록을 표시)
  3. 이모지 렌더링(색상있는 유니코드 사각형 이모지를 사용하여 블록을 표시) : 대부분의 터미널에서 이모지가 겹쳐 보이는 문제가 있습니다.
  4. 터미널 색상 렌더링(#과 : 문자, 터미널 색상 변경으로 블록을 표시)
  5. ASCII 렌더링(모든 출력을 ASCII 문자로 표시)

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

