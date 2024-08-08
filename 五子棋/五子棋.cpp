#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    srand((unsigned int)time(0));

    static char szAppName[] = "HelloXYJJ";
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndclass.lpfnWndProc = (WNDPROC)WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(242, 243, 247));
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, "Ryanzi",
            szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName,                  // window class name
        "xyjj 万岁",		// window caption
        WS_OVERLAPPEDWINDOW,        // window style
        CW_USEDEFAULT,              // initial x position
        CW_USEDEFAULT,              // initial y position
        CW_USEDEFAULT,              // initial x size
        CW_USEDEFAULT,              // initial y size
        NULL,                       // parent window handle
        NULL,                       // window menu handle
        hInstance,                  // program instance handle
        NULL);                     // creation parameters

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

HDC hdc;
PAINTSTRUCT ps;
HBRUSH hs;
HPEN hp;
HFONT hf;
RECT re;
SIZE mysize;

int g[16][16];// 15 * 15 棋盘
int r, a;// 棋子半径, 格子边长
int qcnt;// 已下棋子的个数
int xd, yd;// 鼠标位置
int kx, ky;// 表示应该画框的位置
int qs;// 当前是谁在下，0表示黑子，1表示白子
int ys[2][3] = { {}, { 255, 255, 255 } };// 棋子颜色
int qscore[16][16];
POINT maxqz[260];// 存放ai想放的点
POINT hdd;// 表示需要画红点点的点
POINT la, lb;// 表示胜利需要连线的点 
int mcnt;// 存放ai想放的点的数量
BOOL ai;// 表示是否是人机
BOOL st;// 表示是否处在落子状态
BOOL sai;// 表示现在是否轮到ai来下
BOOL sl;// 是否已经决出胜负
BOOL sf;// 胜方是谁
int wz, wy;
int an[4][3] = { {246, 139, 35}, {161, 84, 17} , {73, 55, 222}, {43, 32, 130} };
int sa1, sa2, sa3, sa4;
RECT an1, an2, an3, an4;
RECT zqz, yqz;
int ym;
char nyl[] = "你赢了！";
char nsl[] = "你输了！";
char wjy[] = "玩家一获胜！";
char wje[] = "玩家二获胜！";
char wzq[] = "五子棋";
char rjdz[] = "人机对战";
char srms[] = "双人模式";
char wj[] = "玩家";
char wj1[] = "玩家1";
char wj2[] = "玩家2";
char rji[] = "人机";
char tc[] = "退出";
char hq[] = "悔棋";

struct qz
{
    int x, y;// 棋子的行和列
    BOOL t;// 棋子类型，0表示黑子，1表示白子
}q[180];

void p_dian(int x, int y)// 画棋盘上的小圆点
{
    hp = CreatePen(PS_SOLID, 1, RGB(190, 161, 119));
    hs = (HBRUSH)GetStockObject(DC_BRUSH);
    SetDCBrushColor(hdc, RGB(190, 161, 119));
    SelectObject(hdc, hp);
    SelectObject(hdc, hs);
    Ellipse(hdc, x - a / 8, y - a / 8, x + a / 8, y + a / 8);
    DeleteObject(hp);
    DeleteObject(hs);
}

POINT getzb(int x, int y)// 通过行列获取实际坐标
{
    POINT p;
    p.y = re.bottom / 2 - 7 * a + (x - 1) * a;
    p.x = re.right / 2 - 7 * a + (y - 1) * a;
    return p;
}

void p_fk(int x, int y)// 通过所给的行列画出对应的下棋方框
{
    POINT o = getzb(x, y);
    POINT zs[] = { {o.x - r, o.y - r + r / 3}, {o.x - r, o.y - r}, {o.x - r + r / 3, o.y - r} };
    POINT ys[] = { {o.x + r - r / 3, o.y - r}, {o.x + r, o.y - r}, {o.x + r, o.y - r + r / 3} };
    POINT zx[] = { {o.x - r, o.y + r - r / 3}, {o.x - r, o.y + r}, {o.x - r + r / 3, o.y + r} };
    POINT yx[] = { {o.x + r - r / 3, o.y + r}, {o.x + r, o.y + r}, {o.x + r, o.y + r - r / 3} };
    hp = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
    hs = (HBRUSH)GetStockObject(DC_BRUSH);
    SetDCBrushColor(hdc, RGB(255, 0, 0));
    SelectObject(hdc, hp);
    SelectObject(hdc, hs);
    Polyline(hdc, zs, 3);
    Polyline(hdc, ys, 3);
    Polyline(hdc, zx, 3);
    Polyline(hdc, yx, 3);
    Ellipse(hdc, o.x - r / 8, o.y - r / 8, o.x + r / 8, o.y + r / 8);
    DeleteObject(hp);
    DeleteObject(hs);
}

void p_qz(int x, int y, int t)// 通过所给的行和列来画棋子
{
    POINT o = getzb(x, y);
    hp = CreatePen(PS_SOLID, 1, RGB(195, 195, 195));
    hs = (HBRUSH)GetStockObject(DC_BRUSH);
    SetDCBrushColor(hdc, RGB(ys[t][0], ys[t][1], ys[t][2]));
    SelectObject(hdc, hp);
    SelectObject(hdc, hs);
    Ellipse(hdc, o.x - r, o.y - r, o.x + r, o.y + r);
    DeleteObject(hp);
    DeleteObject(hs);
}

BOOL pd_sf(int x, int y, int t)
{
    POINT o = getzb(x, y);
    int yz;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0) continue;

            yz = 0, la.x = o.x, la.y = o.y, lb.x = o.x, lb.y = o.y;
            for (int k = 1; k <= 4; k++)
            {
                int xx = x + k * i, yy = y + k * j;

                if (xx < 1 || xx > 15 || yy < 1 || yy > 15) continue;

                if (g[xx][yy] == t + 1)
                {
                    yz++;
                    POINT op = getzb(xx, yy);
                    la.x = op.x, la.y = op.y;
                }
                else
                    break;
            }
            for (int k = 1; k <= 4; k++)
            {
                int xx = x - k * i, yy = y - k * j;

                if (xx < 1 || xx > 15 || yy < 1 || yy > 15) continue;

                if (g[xx][yy] == t + 1)
                {
                    yz++;
                    POINT op = getzb(xx, yy);
                    lb.x = op.x, lb.y = op.y;
                }
                else
                    break;
            }
            if (yz >= 4)
            {
                sf = t, sl = 1;
                return 1;
            }
        }
    }
    return 0;
}

void think(HWND* hwnd)// ai思考过程
{
    for (int i = 1; i <= 15; i++)
        for (int j = 1; j <= 15; j++)
            qscore[i][j] = 0;

    int yz, ky, score, mmax = -1;
    mcnt = 0;
    for (int i = 1; i <= 15; i++)// 行
    {
        for (int j = 1; j <= 15; j++)// 列
        {
            if (g[i][j] != 0) continue;
            score = 0;
            for (int x = -1; x <= 1; x++)
            {
                for (int y = -1; y <= 1; y++)
                {
                    if (x == 0 && y == 0) continue;

                    // 假设该点落黑子
                    yz = ky = 0;
                    for (int k = 1; k <= 4; k++)
                    {
                        int xx = i + k * x, yy = j + k * y;// 行列
                        if (xx < 1 || xx > 15 || yy < 1 || yy > 15) continue;

                        if (g[xx][yy] == 1)// 是黑子
                            yz++;
                        else if (g[xx][yy] == 2)// 是白子
                            break;
                        else // 空余
                        {
                            ky++;
                            break;
                        }
                    }
                    // 往反方向走四步
                    for (int k = 1; k <= 4; k++)
                    {
                        int xx = i - k * x, yy = j - k * y;// 行列
                        if (xx < 1 || xx > 15 || yy < 1 || yy > 15) continue;

                        if (g[xx][yy] == 1)// 是黑子
                            yz++;
                        else if (g[xx][yy] == 2)// 是白子
                            break;
                        else // 空余
                        {
                            ky++;
                            break;
                        }
                    }

                    if (yz == 1) // 连2
                        score += 10;
                    else if (yz == 2)
                    {
                        if (ky == 1) // 死3
                            score += 30;
                        else if (ky == 2) // 活3
                            score += 40;
                    }
                    else if (yz == 3)
                    {
                        if (ky == 1) // 死4
                            score += 60;
                        else if (ky == 2) // 活4
                            score += 200;
                    }
                    else if (yz >= 4)
                        score += 20000;

                    // 假设该点落白子
                    ky = yz = 0;
                    for (int k = 1; k <= 4; k++)
                    {
                        int xx = i + k * x, yy = j + k * y;// 行列
                        if (xx < 1 || xx > 15 || yy < 1 || yy > 15) continue;
                        if (g[xx][yy] == 2)// 是白子
                            yz++;
                        else if (g[xx][yy] == 1)// 是黑子
                            break;
                        else // 空余
                        {
                            ky++;
                            break;
                        }
                    }
                    // 往反方向走四步
                    for (int k = 1; k <= 4; k++)
                    {
                        int xx = i - k * x, yy = j - k * y;// 行列
                        if (xx < 1 || xx > 15 || yy < 1 || yy > 15) continue;

                        if (g[xx][yy] == 2)// 是白子
                            yz++;
                        else if (g[xx][yy] == 1)// 是黑子
                            break;
                        else // 空余
                        {
                            ky++;
                            break;
                        }
                    }

                    if (yz == 0)
                        score += 5;
                    else if (yz == 1) // 连2
                        score += 10;
                    else if (yz == 2)
                    {
                        if (ky == 1) // 死3
                            score += 25;
                        else if (ky == 2) // 活3
                            score += 50;
                    }
                    else if (yz == 3)
                    {
                        if (ky == 1) // 死4
                            score += 55;
                        else if (ky == 2) // 活4
                            score += 10000;
                    }
                    else if (yz >= 4)
                        score += 30000;
                }
            }
            qscore[i][j] = score;
        }
    }

    for (int i = 1; i <= 15; i++)
        for (int j = 1; j <= 15; j++)
        {
            if (mmax < qscore[i][j])
            {
                mmax = qscore[i][j];
                mcnt = 1;
                maxqz[mcnt].x = i, maxqz[mcnt].y = j;
            }
            else if (mmax == qscore[i][j])
                maxqz[++mcnt].x = i, maxqz[mcnt].y = j;
        }

    int u = rand() % mcnt + 1;
    q[++qcnt].x = maxqz[u].x, q[qcnt].y = maxqz[u].y, q[qcnt].t = qs;
    g[maxqz[u].x][maxqz[u].y] = qs + 1;

    qs = 1 - qs;
    POINT o = getzb(maxqz[u].x, maxqz[u].y);
    RECT cq;
    cq.left = o.x - 2 * r, cq.right = o.x + 2 * r, cq.bottom = o.y + 2 * r, cq.top = o.y - 2 * r;
    hdd.x = maxqz[u].x, hdd.y = maxqz[u].y;
    InvalidateRect(*hwnd, &cq, 1);
    POINT pp = getzb(q[qcnt - 1].x, q[qcnt - 1].y);
    RECT pq;
    pq.left = pp.x - 2 * r, pq.right = pp.x + 2 * r, pq.bottom = pp.y + 2 * r, pq.top = pp.y - 2 * r;
    InvalidateRect(*hwnd, &pq, 1);

    if (pd_sf(maxqz[u].x, maxqz[u].y, 1))
        InvalidateRect(*hwnd, NULL, 1);
}

void p_qzb(HWND* hwnd, int x, int y, int t, int w)
{
    hp = CreatePen(PS_SOLID, 3, RGB(195, 195, 195));
    hs = (HBRUSH)GetStockObject(DC_BRUSH);
    SetDCBrushColor(hdc, RGB(ys[t][0], ys[t][1], ys[t][2]));
    SelectObject(hdc, hp);
    SelectObject(hdc, hs);
    Ellipse(hdc, x - w * r, y - w * r, x + w * r, y + w * r);
    DeleteObject(hp);
    DeleteObject(hs);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        GetClientRect(hwnd, &re);
        r = re.bottom / 33, a = re.bottom / 15;
        ym = 1;
        return 0;
    case WM_SIZE:
        GetClientRect(hwnd, &re);
        r = re.bottom / 33, a = re.bottom / 15;
        zqz.left = (re.right / 2 - 7.5 * a) / 2 - 4 * r;
        zqz.right = (re.right / 2 - 7.5 * a) / 2 + 4 * r;
        zqz.top = re.bottom * 3 / 5 - 4 * r;
        zqz.bottom = re.bottom * 3 / 5 + 4 * r;

        yqz.left = (re.right + (re.right / 2 + 7.5 * a)) / 2 - 4 * r;
        yqz.right = (re.right + (re.right / 2 + 7.5 * a)) / 2 + 4 * r;
        yqz.top = re.bottom * 3 / 5 - 4 * r;
        yqz.bottom = re.bottom * 3 / 5 + 4 * r;

        return 0;
    case WM_LBUTTONDOWN:
        if (kx >= 1 && kx <= 15 && ky >= 1 && ky <= 15 && g[kx][ky] == 0 && !sl && ym == 2)// 保证重复点一个地方不会多次进去导致棋子数量超额
        {
            if ((!qs && !sai) || (!ai))// 如果是黑子下棋或者双人下棋
            {
                st = 1;
                q[++qcnt].x = kx, q[qcnt].y = ky, q[qcnt].t = qs;
                g[kx][ky] = qs + 1;
                qs = 1 - qs;
                POINT o = getzb(kx, ky);
                RECT cq;
                cq.left = o.x - 2 * r, cq.right = o.x + 2 * r, cq.bottom = o.y + 2 * r, cq.top = o.y - 2 * r;
                InvalidateRect(hwnd, &cq, 1);
            }
        }
        return 0;
    case WM_LBUTTONUP:
        xd = LOWORD(lParam), yd = HIWORD(lParam);
        if (ym == 1)
        {
            if (xd >= an1.left && xd <= an1.right && yd >= an1.top && yd <= an1.bottom)
            {
                ym = 2;
                InvalidateRect(hwnd, NULL, 1);
                ai = 0, sl = 0;
                hdd.x = -100, hdd.y = -100;
                wz = wy = 2;
                qs = 0;
                sa3 = sa4 = 2;
            }
            else if (xd >= an2.left && xd <= an2.right && yd >= an2.top && yd <= an2.bottom)
            {
                ym = 2;
                InvalidateRect(hwnd, NULL, 1);
                ai = 1, sl = 0;
                hdd.x = -100, hdd.y = -100;
                wz = wy = 2;
                qs = 0;
                sa3 = sa4 = 2;
            }
        }
        else if (st && !sl && ym == 2)
        {
            hdd.x = q[qcnt].x, hdd.y = q[qcnt].y;
            POINT op = getzb(q[qcnt].x, q[qcnt].y);
            RECT lq;
            lq.left = op.x - 2 * r, lq.right = op.x + 2 * r, lq.bottom = op.y + 2 * r, lq.top = op.y - 2 * r;
            InvalidateRect(hwnd, &lq, 1);
            st = 0;
            if (qcnt > 1)
            {
                POINT o = getzb(q[qcnt - 1].x, q[qcnt - 1].y);
                RECT cq;
                cq.left = o.x - 2 * r, cq.right = o.x + 2 * r, cq.bottom = o.y + 2 * r, cq.top = o.y - 2 * r;
                InvalidateRect(hwnd, &cq, 1);
            }
            if (qs)// 如果是黑棋下子
            {
                if (pd_sf(q[qcnt].x, q[qcnt].y, 0))
                    InvalidateRect(hwnd, NULL, 1);
            }
            else
            {
                if (pd_sf(q[qcnt].x, q[qcnt].y, 1))
                    InvalidateRect(hwnd, NULL, 1);
            }
            if (ai)
                sai = 1;
        }
        else if (ym == 2)
        {
            if (xd >= an3.left && xd <= an3.right && yd >= an3.top && yd <= an3.bottom)
            {
                sl = 0;
                sai = 0;
                qs = 0;
                ym = 1;
                qcnt = 0;
                for (int i = 1; i <= 15; i++)
                    for (int j = 1; j <= 15; j++)
                        g[i][j] = 0;
                InvalidateRect(hwnd, NULL, 1);
            }
            if (xd >= an4.left && xd <= an4.right && yd >= an4.top && yd <= an4.bottom)
            {
                if (!ai && qcnt > 0)
                {
                    if (sl) sl = 0;
                    g[q[qcnt].x][q[qcnt].y] = 0;
                    qcnt--;
                    qs = 1 - qs;
                    if (qcnt > 0)
                        hdd.x = q[qcnt].x, hdd.y = q[qcnt].y;
                    else
                        hdd.x = -100, hdd.y = -100;
                    InvalidateRect(hwnd, NULL, 1);
                }
                else if (ai && qcnt % 2 == 0 && qcnt > 0)
                {
                    if (sl) sl = 0;
                    qs = 0;
                    sai = 0;
                    g[q[qcnt].x][q[qcnt].y] = 0;
                    g[q[qcnt - 1].x][q[qcnt - 1].y] = 0;
                    qcnt -= 2;
                    if (qcnt > 0)
                        hdd.x = q[qcnt].x, hdd.y = q[qcnt].y;
                    else
                        hdd.x = -100, hdd.y = -100;
                    InvalidateRect(hwnd, NULL, 1);
                }
            }
        }
        return 0;
    case WM_MOUSEMOVE:
    {
        xd = LOWORD(lParam), yd = HIWORD(lParam);
        if (ym == 1)
        {
            if (xd >= an1.left && xd <= an1.right && yd >= an1.top && yd <= an1.bottom)
            {
                if (sa1 == 0)
                {
                    sa1 = 1, sa2 = 0;
                    InvalidateRect(hwnd, &an1, 1);
                    InvalidateRect(hwnd, &an2, 1);
                }
            }
            else if (xd >= an2.left && xd <= an2.right && yd >= an2.top && yd <= an2.bottom)
            {
                if (sa2 == 0)
                {
                    sa1 = 0, sa2 = 1;
                    InvalidateRect(hwnd, &an1, 1);
                    InvalidateRect(hwnd, &an2, 1);
                }
            }
            else if (sa1 == 1 || sa2 == 1)
            {
                sa1 = sa2 = 0;
                InvalidateRect(hwnd, &an1, 1);
                InvalidateRect(hwnd, &an2, 1);
            }
        }
        else
        {
            int lx = kx, ly = ky;// 上一个鼠标在哪
            kx = ky = 0;
            for (int i = 1; i <= 15; i++)// 行
            {
                for (int j = 1; j <= 15; j++)// 列
                {
                    POINT o = getzb(i, j);
                    if (xd >= o.x - r && xd <= o.x + r && yd >= o.y - r && yd <= o.y + r && g[i][j] == 0)// 如果鼠标在范围，并且当前没有棋子
                    {
                        kx = i, ky = j;
                        break;
                    }
                }
            }
            if ((kx != lx || ky != ly) && !sl && ym == 2)// 现在跟之前的点不一样了
            {
                if (lx >= 1 && lx <= 15 && ly <= 15 && ly >= 1)
                {
                    POINT op = getzb(lx, ly);
                    RECT lq;
                    lq.left = op.x - 2 * r, lq.right = op.x + 2 * r, lq.bottom = op.y + 2 * r, lq.top = op.y - 2 * r;
                    InvalidateRect(hwnd, &lq, 1);
                }
                if (kx >= 1 && kx <= 15 && ky >= 1 && ky <= 15)
                {
                    POINT o = getzb(kx, ky);
                    RECT cq;
                    cq.left = o.x - 2 * r, cq.right = o.x + 2 * r, cq.bottom = o.y + 2 * r, cq.top = o.y - 2 * r;
                    InvalidateRect(hwnd, &cq, 1);
                }

            }

            if (xd >= an3.left && xd <= an3.right && yd >= an3.top && yd <= an3.bottom)
            {
                if (sa3 != 3)
                {
                    sa3 = 3;
                    InvalidateRect(hwnd, &an3, 1);
                }
            }
            else if (sa3 == 3)
            {
                sa3 = 2;
                InvalidateRect(hwnd, &an3, 1);
            }

            if (xd >= an4.left && xd <= an4.right && yd >= an4.top && yd <= an4.bottom)
            {
                if (sa4 != 3)
                {
                    sa4 = 3;
                    InvalidateRect(hwnd, &an4, 1);
                }
            }
            else if (sa4 == 3)
            {
                sa4 = 2;
                InvalidateRect(hwnd, &an4, 1);
            }
        }
        return 0;
    }
    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &re);

        if (ym == 1)// 菜单页面
        {
            hs = (HBRUSH)GetStockObject(DC_BRUSH);
            SetDCBrushColor(hdc, RGB(246, 163, 67));
            SelectObject(hdc, hs);
            Rectangle(hdc, re.left, re.top, re.right, re.bottom);
            DeleteObject(hs);

            hf = CreateFont(re.bottom / 10, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            SelectObject(hdc, hf);
            SetBkColor(hdc, RGB(an[sa1][0], an[sa1][1], an[sa1][2]));
            GetTextExtentPoint32(hdc, srms, strlen(srms), &mysize);
            TextOut(hdc, re.right / 2 - mysize.cx / 2, re.bottom / 2 - mysize.cy / 2, srms, strlen(srms));
            an1.left = re.right / 2 - mysize.cx / 2;
            an1.right = re.right / 2 + mysize.cx / 2;
            an1.top = re.bottom / 2 - mysize.cy / 2;
            an1.bottom = re.bottom / 2 + mysize.cy / 2;
            DeleteObject(hf);

            hf = CreateFont(re.bottom / 10, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            SelectObject(hdc, hf);
            SetBkColor(hdc, RGB(an[sa2][0], an[sa2][1], an[sa2][2]));
            GetTextExtentPoint32(hdc, rjdz, strlen(rjdz), &mysize);
            TextOut(hdc, re.right / 2 - mysize.cx / 2, re.bottom * 2 / 3 - mysize.cy / 2, rjdz, strlen(rjdz));
            an2.left = re.right / 2 - mysize.cx / 2;
            an2.right = re.right / 2 + mysize.cx / 2;
            an2.top = re.bottom * 2 / 3 - mysize.cy / 2;
            an2.bottom = re.bottom * 2 / 3 + mysize.cy / 2;
            DeleteObject(hf);

            hf = CreateFont(re.bottom / 5, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            SelectObject(hdc, hf);
            SetBkColor(hdc, RGB(246, 163, 67));
            GetTextExtentPoint32(hdc, wzq, strlen(wzq), &mysize);
            TextOut(hdc, re.right / 2 - mysize.cx / 2, re.bottom / 4 - mysize.cy / 2, wzq, strlen(wzq));
            DeleteObject(hf);
        }
        else
        {
            // 画棋盘
            hp = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
            hs = (HBRUSH)GetStockObject(DC_BRUSH);
            SetDCBrushColor(hdc, RGB(247, 231, 182));
            SelectObject(hdc, hp);
            SelectObject(hdc, hs);
            RoundRect(hdc, re.right / 2 - 7.5 * a, re.bottom / 2 - 7.5 * a, re.right / 2 + 7.5 * a, re.bottom / 2 + 7.5 * a, 30, 30);
            DeleteObject(hp);
            DeleteObject(hs);
            hp = CreatePen(PS_SOLID, 3, RGB(190, 161, 119));
            SelectObject(hdc, hp);
            for (int i = 0; i < 15; i++)
            {
                MoveToEx(hdc, re.right / 2 - 7 * a + i * a, re.bottom / 2 - 7 * a, NULL);
                LineTo(hdc, re.right / 2 - 7 * a + i * a, re.bottom / 2 + 7 * a);
                MoveToEx(hdc, re.right / 2 - 7 * a, re.bottom / 2 - 7 * a + i * a, NULL);
                LineTo(hdc, re.right / 2 + 7 * a, re.bottom / 2 - 7 * a + i * a);
            }
            DeleteObject(hp);
            p_dian(re.right / 2 - 4 * a, re.bottom / 2 - 4 * a);
            p_dian(re.right / 2 + 4 * a, re.bottom / 2 - 4 * a);
            p_dian(re.right / 2 - 4 * a, re.bottom / 2 + 4 * a);
            p_dian(re.right / 2 + 4 * a, re.bottom / 2 + 4 * a);
            p_dian(re.right / 2, re.bottom / 2);

            if (kx >= 1 && kx <= 15 && ky >= 1 && ky <= 15 && g[kx][ky] == 0 && !sl)
                p_fk(kx, ky);

            // 画棋子
            for (int i = 1; i <= 15; i++)
                for (int j = 1; j <= 15; j++)
                {
                    if (g[i][j] == 1)// 黑子
                        p_qz(i, j, 0);
                    else if (g[i][j] == 2)// 白子 
                        p_qz(i, j, 1);
                }

            // 画左右两边
            if (ai)
            {
                hf = CreateFont(re.bottom / 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                SelectObject(hdc, hf);
                SetBkColor(hdc, RGB(242, 243, 247));
                GetTextExtentPoint32(hdc, rji, strlen(rji), &mysize);
                TextOut(hdc, (re.right / 2 - 7.5 * a) / 2 - mysize.cx / 2, re.bottom / 3 - mysize.cy / 2, rji, strlen(rji));
                DeleteObject(hf);

                hf = CreateFont(re.bottom / 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                SelectObject(hdc, hf);
                SetBkColor(hdc, RGB(242, 243, 247));
                GetTextExtentPoint32(hdc, wj, strlen(wj), &mysize);
                TextOut(hdc, (re.right + (re.right / 2 + 7.5 * a)) / 2 - mysize.cx / 2, re.bottom / 3 - mysize.cy / 2, wj, strlen(wj));
                DeleteObject(hf);

                p_qzb(&hwnd, (re.right / 2 - 7.5 * a) / 2, re.bottom * 3 / 5, 1, wz);
                p_qzb(&hwnd, (re.right + (re.right / 2 + 7.5 * a)) / 2, re.bottom * 3 / 5, 0, wy);

                if (qs && wz != 3)
                {
                    wz = 3, wy = 2;
                    InvalidateRect(hwnd, &zqz, 1);
                    InvalidateRect(hwnd, &yqz, 1);
                }
                else if (!qs && wy != 3)
                {
                    wz = 2, wy = 3;
                    InvalidateRect(hwnd, &zqz, 1);
                    InvalidateRect(hwnd, &yqz, 1);
                }
            }
            else
            {
                hf = CreateFont(re.bottom / 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                SelectObject(hdc, hf);
                SetBkColor(hdc, RGB(242, 243, 247));
                GetTextExtentPoint32(hdc, wj2, strlen(wj2), &mysize);
                TextOut(hdc, (re.right / 2 - 7.5 * a) / 2 - mysize.cx / 2, re.bottom / 3 - mysize.cy / 2, wj2, strlen(wj2));
                DeleteObject(hf);

                hf = CreateFont(re.bottom / 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                SelectObject(hdc, hf);
                SetBkColor(hdc, RGB(242, 243, 247));
                GetTextExtentPoint32(hdc, wj1, strlen(wj1), &mysize);
                TextOut(hdc, (re.right + (re.right / 2 + 7.5 * a)) / 2 - mysize.cx / 2, re.bottom / 3 - mysize.cy / 2, wj1, strlen(wj1));
                DeleteObject(hf);

                p_qzb(&hwnd, (re.right / 2 - 7.5 * a) / 2, re.bottom * 3 / 5, 1, wz);
                p_qzb(&hwnd, (re.right + (re.right / 2 + 7.5 * a)) / 2, re.bottom * 3 / 5, 0, wy);

                if (qs && wz != 3)
                {
                    wz = 3, wy = 2;
                    InvalidateRect(hwnd, &zqz, 1);
                    InvalidateRect(hwnd, &yqz, 1);
                }
                else if (!qs && wy != 3)
                {
                    wz = 2, wy = 3;
                    InvalidateRect(hwnd, &zqz, 1);
                    InvalidateRect(hwnd, &yqz, 1);
                }
            }

            if (qcnt >= 1)// 画上一个棋子的红点点
            {
                POINT o = getzb(hdd.x, hdd.y);
                hp = CreatePen(PS_SOLID, 1, RGB(0, 162, 232));
                SelectObject(hdc, hp);
                hs = (HBRUSH)GetStockObject(DC_BRUSH);
                SetDCBrushColor(hdc, RGB(0, 162, 232));
                SelectObject(hdc, hs);
                Ellipse(hdc, o.x - r / 5, o.y - r / 5, o.x + r / 5, o.y + r / 5);
                DeleteObject(hp);
                DeleteObject(hs);
            }

            // 画退出按钮
            hf = CreateFont(re.bottom / 10, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            SelectObject(hdc, hf);
            SetTextColor(hdc, RGB(an[sa3][0], an[sa3][1], an[sa3][2]));
            SetBkMode(hdc, 1);
            GetTextExtentPoint32(hdc, tc, strlen(tc), &mysize);
            TextOut(hdc, 0, 0, tc, strlen(tc));
            an3.left = 0;
            an3.right = mysize.cx;
            an3.top = 0;
            an3.bottom = mysize.cy;
            DeleteObject(hf);

            // 画悔棋按钮
            hf = CreateFont(re.bottom / 10, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            SelectObject(hdc, hf);
            SetTextColor(hdc, RGB(an[sa4][0], an[sa4][1], an[sa4][2]));
            SetBkMode(hdc, 1);
            GetTextExtentPoint32(hdc, hq, strlen(hq), &mysize);
            TextOut(hdc, re.right - mysize.cx, 0, hq, strlen(hq));
            an4.left = re.right - mysize.cx;
            an4.right = re.right;
            an4.top = 0;
            an4.bottom = mysize.cy;
            DeleteObject(hf);

            if (sl)// 如果胜利了，就对胜利点进行连线，并且显示结算画面
            {
                hp = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
                SelectObject(hdc, hp);
                MoveToEx(hdc, la.x, la.y, NULL);
                LineTo(hdc, lb.x, lb.y);
                DeleteObject(hp);

                if (ai && sf == 0)
                {
                    hf = CreateFont(re.bottom / 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    SelectObject(hdc, hf);
                    SetBkMode(hdc, TRANSPARENT);
                    SetTextColor(hdc, RGB(76, 57, 232));
                    GetTextExtentPoint32(hdc, nyl, strlen(nyl), &mysize);
                    TextOut(hdc, re.right / 2 - mysize.cx / 2, re.bottom / 2 - mysize.cy / 2, nyl, strlen(nyl));
                    DeleteObject(hf);
                }
                else if (ai && sf == 1)
                {
                    hf = CreateFont(re.bottom / 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    SelectObject(hdc, hf);
                    SetTextColor(hdc, RGB(76, 57, 232));

                    SetBkMode(hdc, TRANSPARENT);
                    GetTextExtentPoint32(hdc, nsl, strlen(nsl), &mysize);
                    TextOut(hdc, re.right / 2 - mysize.cx / 2, re.bottom / 2 - mysize.cy / 2, nsl, strlen(nsl));
                    DeleteObject(hf);
                }
                else if (!ai && sf == 0)
                {
                    hf = CreateFont(re.bottom / 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    SelectObject(hdc, hf);
                    SetTextColor(hdc, RGB(76, 57, 232));

                    SetBkMode(hdc, TRANSPARENT);
                    GetTextExtentPoint32(hdc, wjy, strlen(wjy), &mysize);
                    TextOut(hdc, re.right / 2 - mysize.cx / 2, re.bottom / 2 - mysize.cy / 2, wjy, strlen(wjy));
                    DeleteObject(hf);
                }
                else if (!ai && sf == 1)
                {
                    hf = CreateFont(re.bottom / 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                    SelectObject(hdc, hf);
                    SetTextColor(hdc, RGB(76, 57, 232));

                    SetBkMode(hdc, TRANSPARENT);
                    GetTextExtentPoint32(hdc, wje, strlen(wje), &mysize);
                    TextOut(hdc, re.right / 2 - mysize.cx / 2, re.bottom / 2 - mysize.cy / 2, wje, strlen(wje));
                    DeleteObject(hf);
                }
            }

            if (qs && ai && sai && !sl)// 如果是等待ai下子
            {
                Sleep(800);
                think(&hwnd);
                sai = 0;
            }
        }

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
