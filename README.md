# Omok Game(WinAPI 기반 C 구현)

C 언어와 Win32 API를 활용하여 구현한 오목(Gomoku) 게임입니다.  
플레이어 대전(PvP), 인공지능 대전(PvE), 난이도 선택 기능 등을 포함하고 있습니다.



## 프로젝트 구조
```
project
├── main.c # 진입점
├── omok_ai.c / .h # AI 수 계산 및 난이도 로직
├── omok_logic.c / .h # 승리 조건, 규칙 처리
├── omok_ui.c / .h # UI 및 입력 처리
├── omok_global.h # 전역 변수 모음
├── project.vcxproj # Visual Studio 프로젝트 파일
├── .gitignore # 빌드 파일 제외 설정
└── (x64/, Debug/ 등 빌드 산출물은 Git에서 제외됨)
```


## 빌드 환경

- OS: Windows 10 이상
- IDE: Visual Studio 2022
- 언어: C / Win32 API



## 설정

- **링커 → 시스템**: `Windows (/SUBSYSTEM:WINDOWS)`
- **전처리기 정의**: `_CRT_SECURE_NO_WARNINGS; WIN32_LEAN_AND_MEAN`
- **필수 라이브러리**: `user32.lib`, `gdi32.lib`, `comdlg32.lib`
