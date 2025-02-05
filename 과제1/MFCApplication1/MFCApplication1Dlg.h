#pragma once  // 중복 포함 방지

#include <vector>    // 동적 배열을 위한 STL 벡터 포함
#include <gdiplus.h> // GDI+ 사용을 위한 헤더 포함
#include <thread>    // 멀티스레딩을 위한 헤더 포함
#include <random>    // 랜덤 숫자 생성을 위한 헤더 포함
#include <afxwin.h>  // MFC 기본 윈도우 헤더 추가 (CEdit 사용을 위해 필요)

using namespace Gdiplus;  // GDI+ 네임스페이스 사용

// CMFCApplication1Dlg 클래스 선언
class CMFCApplication1Dlg : public CDialogEx
{
public:
    // 생성자: 다이얼로그 초기화
    CMFCApplication1Dlg(CWnd* pParent = nullptr);

protected:
    // MFC 메시지 처리 함수
    virtual BOOL OnInitDialog();   // 다이얼로그 초기화 함수
    afx_msg void OnPaint();        // 화면을 다시 그리는 함수
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);  // 배경 지우기 함수 (깜빡임 방지)

    // 마우스 이벤트 핸들러
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);  // 마우스 왼쪽 버튼 클릭
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);    // 마우스 이동
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);    // 마우스 버튼 해제

    // 버튼 클릭 이벤트 핸들러
    afx_msg void OnBnClickedReset();       // 초기화 버튼 클릭
    afx_msg void OnBnClickedRandomMove();  // 랜덤 이동 버튼 클릭
    afx_msg void OnBnClickedSetRadius();   // 반지름 설정 버튼 클릭
    afx_msg void OnBnClickedSetBorder();   // 테두리 두께 설정 버튼 클릭

    // 데이터 교환 함수 (MFC에서 UI와 데이터 바인딩)
    virtual void DoDataExchange(CDataExchange* pDX);

    // 메시지 맵 선언 (MFC에서 이벤트 핸들러 등록)
    DECLARE_MESSAGE_MAP()

private:
    // 점(클릭한 위치) 저장 벡터
    std::vector<CPoint> points;  // 사용자가 클릭한 점 저장
    CPoint selectedPoint;        // 사용자가 현재 드래그 중인 점

    // 드래그 상태 및 UI 설정 변수
    bool isDragging;      // 드래그 여부 확인
    int radius;           // 원의 반지름
    int borderWidth;      // 원의 테두리 두께
    bool isRunning;       // 랜덤 이동 실행 여부

    // 랜덤 이동을 위한 스레드
    std::thread randomMoveThread;

    // 더블 버퍼링을 위한 변수 (화면 깜빡임 방지)
    CBitmap m_Bitmap;  // 백 버퍼용 비트맵
    CDC m_MemDC;       // 백 버퍼용 DC

    // 주요 기능을 수행하는 내부 함수
    void DrawScene(Graphics& graphics);            // 원과 점을 그리는 함수
    void CalculateCircle(CPoint& center, int& circleRadius); // 원의 중심과 반지름 계산
    void StartRandomMovement();                    // 점을 랜덤한 위치로 이동

    // ✅ 추가: 버튼 영역을 제외하고 다시 그리는 함수 선언
    void RedrawNonButtonArea();

    // ✅ 추가: 사용자 입력을 받기 위한 CEdit 컨트롤
    CEdit m_editRadius;   // 반지름 입력 필드
    CEdit m_editBorder;   // 테두리 두께 입력 필드
};
