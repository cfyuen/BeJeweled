#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <windows.h>
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <queue>
using namespace std;

struct Color {
    int r,g,b;
    bool operator < (const Color c) const {
        return c.r < r;
    }
};

HDC _hdc;
HWND beJeweledWindow;
RECT r;
POINT corner;
const int N = 8;
const int C = 17;
int PIX = 40; // pix should be flexible
int bd[N][N];
Color gem[C];
int parCol[C];
FILE *debug;


Color toColor(int r,int g,int b) {
    Color ret;
    ret.r = r; ret.g = g; ret.b = b;
    return ret;
}

int colDist(Color a,Color b) {
    return (a.b-b.b)*(a.b-b.b)+(a.g-b.g)*(a.g-b.g)+(a.r-b.r)*(a.r-b.r);
}

Color getScreenPixel(int x,int y,RECT r) {
    Color c;
    POINT _cursor, init;
    _cursor.x = x - r.left; _cursor.y = y - r.top;
    init.x = 0;
    init.y = 0;
    _hdc = GetWindowDC(WindowFromPoint(init));
    //_hdc = GetWindowDC(NULL);
    DWORD _color = GetPixel(_hdc, _cursor.x, _cursor.y);
    c.r = GetRValue(_color);
    c.g = GetGValue(_color);
    c.b = GetBValue(_color);
    return c;
}

Color getScreenPixel(int x,int y) {
    RECT r;
    r.top = 0; r.left = 0;
    return getScreenPixel(x,y,r);
}

void utilities() {
    //get position of a point
    POINT p;
    do {
        Sleep(100);
        if (GetAsyncKeyState(VK_RBUTTON) < 0) {

            GetCursorPos(&p);
            printf("hi %d,%d\n",p.x,p.y); fflush(stdout);
        }
    } while (true);
}

void setColor() {
    gem[0] = toColor(254,254,254); // white
    gem[1] = toColor(254,235,35); // yellow
    gem[2] = toColor(22,119,239); // blue
    gem[3] = toColor(228,13,228); // purple
    gem[4] = toColor(98,254,156); // green
    gem[5] = toColor(248,26,54); // red
    gem[6] = toColor(254,254,131); // orange
    gem[7] = toColor(237,93,18); // orange
    gem[8] = toColor(5,138,18); // green
    gem[9] = toColor(0,62,173); // blue
    gem[10] = toColor(220,220,220); // white
    gem[11] = toColor(6,192,24); // green
    gem[12] = toColor(7,99,223); // blue
    gem[13] = toColor(179,138,32); // yellow
    gem[14] = toColor(199,166,46); // yellow
    gem[15] = toColor(53,15,5); // black box
    gem[16] = toColor(122,122,122); // white x2
    for (int i=0; i<7; i++) parCol[i] = i;
    parCol[7] = 6;
    parCol[8] = 4;
    parCol[9] = 2;
    parCol[10] = 0;
    parCol[11] = 4;
    parCol[12] = 2;
    parCol[13] = 1;
    parCol[14] = 1;
    parCol[15] = 7;
    parCol[16] = 0;
}

void init() {
    setColor();

	do {
		//beJeweledWindow = FindWindow(NULL,"New Tab - Google Chrome");
		beJeweledWindow = FindWindow(NULL,"Bejeweled Blitz on Facebook - Google Chrome");
		Sleep(10);
	} while (beJeweledWindow == NULL);

	ShowWindow (beJeweledWindow, SW_MAXIMIZE);

//    char str[500];
//    GetClassName(floodItWindow,str,500);
//    printf("%s\n",str);

	Sleep(200);
	bool ok = SetForegroundWindow(beJeweledWindow);
	//printf("%d\n",ok);
	Sleep(200);
	GetWindowRect(beJeweledWindow, &r);
	RECT cr;
	GetClientRect(beJeweledWindow, &cr);

    //printf("%d %d %d %d\n",r.left,r.right,r.top,r.bottom);
    //printf("%d %d %d %d\n",cr.left,cr.right,cr.top,cr.bottom);
    Color col;
    Sleep(100);
    for (int i=100; i<280; i+=4) {
        for (int j=0; j<200; j+=4) {
            col = getScreenPixel(j,i,r);
            if (col.r == col.g && col.g == col.b && col.r != 255) {
                corner.x = j; corner.y = i;
                //fprintf(debug,"%d,%d: %d,%d,%d   ",j,i,col.r,col.g,col.b);
                //SetCursorPos(j,i); Sleep(200);
                goto end;
            }

        }
        //fprintf(debug,"\n");
    }
    end:

    corner.x += 175;
    corner.y += 55;


//    for (int i=0; i<N; i++)
//        for (int j=0; j<N; j++) {
//            int x = corner.x + i*PIX;
//            int y = corner.y + j*PIX;
//            SetCursorPos(x,y); Sleep(50);
//        }


}

void destroy() {
    //ShowWindow (beJeweledWindow, SW_MINIMIZE);
}


/* Play Game and Solve Game */

int dx[] = {0,1,0,-1,0,-1,1},dy[] = {1,0,-1,0,0,1,-1};

bool getBoard() {
    int fail = 0,tot[C];
    int toterr;
    int isCol[C];
    do {
        Color col;
        memset(tot,0,sizeof(tot));
        toterr = 0;
        for (int i=0; i<N; i++) {
            for (int j=0; j<N; j++) {
                int best,dis,bestnon0;
                best = (1<<30);
                bestnon0 = (1<<30);
                bd[i][j] = -1;
                memset(isCol,127,sizeof(isCol));
                for (int d=2; d<7; d++) {
                    int x = corner.x + i*PIX + PIX/2 + dx[d]*rand()%10;
                    int y = corner.y + j*PIX + PIX/2 + dy[d]*rand()%10;
                    col = getScreenPixel(x,y,r);
                    //SetCursorPos(x,y); Sleep(100);
                    for (int k=0; k<C; k++) {
                        dis = colDist(col,gem[k]);
                        //fprintf(debug,"%d,%d (%d,%d,%d) %d = %d\n",j,i,col.r,col.g,col.b,parCol[k],dis);
                        if (dis < best) {
                            best = dis;
                            //bd[i][j] = parCol[k];
                        }
                        isCol[parCol[k]] = min(dis,isCol[parCol[k]]);
                        if (parCol[k] != 0) {
                            bestnon0 = min(bestnon0,dis);
                        }
                    }
                }
                toterr += best;
                for (int k=0; k<8; k++) {
                    if (isCol[k] < 1000) {
                        bd[i][j] = k;
                        if (k > 0 && isCol[k] == bestnon0) break;
                    }
                }
                if (bd[i][j] != -1) {
                    tot[bd[i][j]]++;
                    if (tot[bd[i][j]] >= 36) {
                        fail = 5;
                    }
                }
                //fprintf(debug,"%d(%d - %d,%d,%d) ",bd[i][j],best,col.r,col.g,col.b);
                //fprintf(debug,"%2d ",bd[i][j],best,col.r,col.g,col.b);
            }
            fprintf(debug,"\n");
        }
        for (int i=0; i<N; i++) {
            for (int j=0; j<N; j++)
                fprintf(debug,"%2d ",bd[j][i]);
            fprintf(debug,"\n");
        }
        fprintf(debug,"%d\n",toterr);
        if (toterr > 50000) {
            fail++;
            Sleep(50);
        }
    } while (toterr > 50000 && fail < 5);
    if (fail >= 5) return false;
    return true;
}


int evaluate(int x,int y) {
    int nx,ny,sx,sy,cnt;
    int sco = 0;
    for (int i=2; i<4; i++) {
        sx = x+dx[i]*3;
        sy = y+dy[i]*3;
        for (int j=0; j<3; j++) {
            sx += dx[i-2];
            sy += dy[i-2];
            cnt = 0;
            for (int k=0; k<5; k++) {
                nx = sx + dx[i-2]*k;
                ny = sy + dy[i-2]*k;
                if (nx >= 0 && ny >= 0 && nx < N && ny < N && bd[nx][ny] == bd[x][y]) {
                    cnt++;
                }
                else break;
            }
            //fprintf(debug,"%d,%d %d,%d  = %d\n",sx,sy,dx[i-2],dy[i-2],cnt);
            if (cnt >= 3) {
                break;
            }
        }
        if (cnt == 3) sco += 50;
        if (cnt == 4) sco += 500;
        if (cnt == 5) sco += 2000;
    }

//    for (int i=0; i<N; i++) {
//        for (int j=0; j<N; j++) {
//            fprintf(debug,"%d ",bd[i][j]);
//        }
//        fprintf(debug,"\n");
//    }
    //fprintf(debug,"  %d,%d %d\n",x,y,sco);
    return sco;
}

void getSwap(int &mx,int &my,int &mk,int lmx,int lmy) {
    int sco,best = 0;
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++) {
            for (int k=0; k<2; k++) {
                int nx = i+dx[k],ny = j+dy[k];
                if (nx < N && ny < N && bd[i][j] != bd[nx][ny]) {
                    swap(bd[i][j],bd[nx][ny]);
                    sco = 0;
                    sco += evaluate(i,j);
                    sco += evaluate(nx,ny);
                    sco += j;
                    sco += abs(i-lmx)+abs(j-lmy);
                    if (sco > best) {
                        //printf("best %d\n",sco);
                        best = sco;
                        mx = i; my = j; mk = k;
                    }
                    swap(bd[i][j],bd[nx][ny]);
                }
            }
        }
}

void doSwap(int mx,int my,int mk) {
    int nx,ny;
    nx = mx + dx[mk];
    ny = my + dy[mk];
    fprintf(debug,"Move: %d,%d <-> %d,%d\n",my,mx,ny,nx);
    SetCursorPos(corner.x + mx*PIX + PIX/2,corner.y + my*PIX + PIX/2);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    Sleep(30);
    SetCursorPos(corner.x + nx*PIX + PIX/2,corner.y + ny*PIX + PIX/2);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    Sleep(30);
}

void playGame() {
    int mx,my,mk,lmx = 0,lmy = 0;
    while (getBoard()) {
        getSwap(mx,my,mk,lmx,lmy);
        lmx = mx; lmy = my;
        doSwap(mx,my,mk);
        Sleep(100);
    }
//    getBoard();
//    getSwap(mx,my,mk);
//    doSwap(mx,my,mk);
}

int main() {
    debug = fopen("debug.txt","w");
    //utilities();
    init();
    int cnt = 0;
    do {
        printf("Game #%d\n",cnt);

        playGame();
        cnt ++;
    } while (cnt < 1);
    destroy();
}
