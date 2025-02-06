#include "pch.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include <random>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 생성자: 초기값 설정
// 
// - 초기 반지름(radius): 10
// - 초기 테두리 두께(borderWidth): 2
// - 드래그 상태(isDragging): false (초기에는 드래그 비활성화)
// - 랜덤 이동 실행 여부(isRunning): false (랜덤 이동 미실행)

CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent), isDragging(false), radius(10), borderWidth(2), isRunning(false)
{
}

// 데이터 교환 함수
void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);  
    DDX_Control(pDX, IDC_EDIT_RADIUS, m_editRadius);  // 반지름 입력칸을 m_editRadius 멤버 변수와 연결
    DDX_Control(pDX, IDC_EDIT_BORDER, m_editBorder);  // 테두리 두께 입력칸을 m_editBorder 멤버 변수와 연결
}

// 메시지 맵 설정
BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
    ON_WM_PAINT()               // 화면 다시 그리기 처리
    ON_WM_ERASEBKGND()          // 배경 지우기 처리
    ON_WM_LBUTTONDOWN()         // 마우스 왼쪽 버튼 클릭
    ON_WM_MOUSEMOVE()           // 마우스 이동 
    ON_WM_LBUTTONUP()           // 마우스 버튼 해제

    ON_BN_CLICKED(IDC_BUTTON_RESET, &CMFCApplication1Dlg::OnBnClickedReset)           // 초기화
    ON_BN_CLICKED(IDC_BUTTON_RANDOM, &CMFCApplication1Dlg::OnBnClickedRandomMove)     // 랜덤 이동
    ON_BN_CLICKED(IDC_BUTTON_SET_RADIUS, &CMFCApplication1Dlg::OnBnClickedSetRadius)  // 반지름 적용
    ON_BN_CLICKED(IDC_BUTTON_SET_BORDER, &CMFCApplication1Dlg::OnBnClickedSetBorder)  // 원의 두께 적용
END_MESSAGE_MAP()

// 다이얼로그 초기화
BOOL CMFCApplication1Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
 
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr); 

    return TRUE; 
}

// 배경 깜빡임 방지
BOOL CMFCApplication1Dlg::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;  
}


// 화면을 다시 그리는 함수
void CMFCApplication1Dlg::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);

    //더블 버퍼링 설정(깜빡임 방지)
    if (!m_Bitmap.GetSafeHandle())
    {
        m_MemDC.CreateCompatibleDC(&dc);
        m_Bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
        m_MemDC.SelectObject(&m_Bitmap);
    }

    Graphics graphics(m_MemDC);
    graphics.Clear(Color(255, 255, 255, 255));
    DrawScene(graphics);

    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &m_MemDC, 0, 0, SRCCOPY);
}

// 마우스 클릭 (점 추가 및 드래그 시작)
void CMFCApplication1Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    for (auto& p : points)
    {
        if ((abs(point.x - p.x) < 10) && (abs(point.y - p.y) < 10))
        {
            selectedPoint = p;
            isDragging = true;
            break;
        }
    }

    if (!isDragging && points.size() < 3)
    {
        points.push_back(point);
        RedrawNonButtonArea();
    }

    CDialogEx::OnLButtonDown(nFlags, point);
}

// 마우스 이동 핸들러 (점 드래그)
void CMFCApplication1Dlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (isDragging)
    {
        for (auto& p : points)
        {
            if (p == selectedPoint)
            {
                p = point;
                selectedPoint = point;
                break;
            }
        }
        RedrawNonButtonArea();
    }

    CDialogEx::OnMouseMove(nFlags, point);
}

// 마우스 버튼 해제 (드래그 종료)
void CMFCApplication1Dlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    isDragging = false;
    RedrawNonButtonArea();
    CDialogEx::OnLButtonUp(nFlags, point);
}

// 화면 그리기
void CMFCApplication1Dlg::DrawScene(Graphics& graphics)
{
    Pen blackPen(Color(0, 0, 0), borderWidth);
    SolidBrush blackBrush(Color(0, 0, 0));

    for (const auto& p : points)
    {
        graphics.FillEllipse(&blackBrush, p.x - radius, p.y - radius, radius * 2, radius * 2);
    }

    if (points.size() == 3)
    {
        CPoint center;
        int circleRadius;
        CalculateCircle(center, circleRadius);
        graphics.DrawEllipse(&blackPen, center.x - circleRadius, center.y - circleRadius, circleRadius * 2, circleRadius * 2);
    }
}

// 원 계산
void CMFCApplication1Dlg::CalculateCircle(CPoint& center, int& circleRadius)
{
    if (points.size() != 3) return;

    CPoint p1 = points[0], p2 = points[1], p3 = points[2];

    double a1 = p2.x - p1.x, b1 = p2.y - p1.y;
    double a2 = p3.x - p1.x, b2 = p3.y - p1.y;
    double d = 2 * (a1 * b2 - b1 * a2);

    if (d == 0) return;

    double cx = ((b2 * (a1 * a1 + b1 * b1)) - (b1 * (a2 * a2 + b2 * b2))) / d;
    double cy = ((a1 * (a2 * a2 + b2 * b2)) - (a2 * (a1 * a1 + b1 * b1))) / d;

    center = CPoint((int)(p1.x + cx), (int)(p1.y + cy));
    circleRadius = (int)sqrt(cx * cx + cy * cy);
}

// '초기화' 버튼 

void CMFCApplication1Dlg::OnBnClickedReset()
{
    points.clear();
    isDragging = false;
    RedrawNonButtonArea();
}

// 랜덤 이동 실행
void CMFCApplication1Dlg::StartRandomMovement()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distX(50, 300);
    std::uniform_int_distribution<int> distY(50, 300);

    for (int i = 0; i < 10; ++i)
    {
        Sleep(500);
        for (auto& p : points)
        {
            p.x = distX(gen);
            p.y = distY(gen);
        }
        RedrawNonButtonArea();
    }

    isRunning = false;
}

// 랜덤 이동 실행
void CMFCApplication1Dlg::OnBnClickedRandomMove()
{
    if (isRunning) return;

    isRunning = true;
    randomMoveThread = std::thread(&CMFCApplication1Dlg::StartRandomMovement, this);
    randomMoveThread.detach();
}

// 버튼 클릭: 반지름 변경
void CMFCApplication1Dlg::OnBnClickedSetRadius()
{
    CString str;
    m_editRadius.GetWindowText(str);
    int inputValue = _ttoi(str);

    if (inputValue > 0)
    {
        radius = inputValue;

        // 입력칸의 깜빡임을 방지
        m_editRadius.SetRedraw(FALSE);
        RedrawNonButtonArea();
        m_editRadius.SetRedraw(TRUE);
        m_editRadius.Invalidate(FALSE);
    }
}

// 버튼 클릭 : 테두리 두께 변경
void CMFCApplication1Dlg::OnBnClickedSetBorder()
{
    CString str;
    m_editBorder.GetWindowText(str);
    int inputValue = _ttoi(str);

    if (inputValue > 0)
    {
        borderWidth = inputValue;
        RedrawNonButtonArea();
    }
}

void CMFCApplication1Dlg::RedrawNonButtonArea()
{
    CRect clientRect;
    GetClientRect(&clientRect);

    // 버튼과 입력칸을 포함한 모든 컨트롤 ID 목록
    int excludeIds[] = { IDC_BUTTON_RESET, IDC_BUTTON_RANDOM, IDC_BUTTON_SET_RADIUS, IDC_BUTTON_SET_BORDER, IDC_EDIT_RADIUS, IDC_EDIT_BORDER };

    CRgn updateRegion;
    updateRegion.CreateRectRgn(clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);

    for (int id : excludeIds)
    {
        CWnd* pWnd = GetDlgItem(id);
        if (pWnd)
        {
            CRect excludeRect;
            pWnd->GetWindowRect(&excludeRect);
            ScreenToClient(&excludeRect);

            // 해당 버튼이 있는 영역을 업데이트에서 제외
            CRgn excludeRegion;
            excludeRegion.CreateRectRgn(excludeRect.left, excludeRect.top, excludeRect.right, excludeRect.bottom);
            updateRegion.CombineRgn(&updateRegion, &excludeRegion, RGN_DIFF);
        }
    }

    // RedrawWindow 함수에 CRgn* 전달 (오류 해결)
    RedrawWindow(nullptr, &updateRegion, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
}
