#include <stdio.h>
#include <stdlib.h>
#include <libps.h>
#include <string.h>
#include <pad.h>

/* Globals */
#define SCREEN_W   320
#define SCREEN_H   240

#define OT_LENGTH  12
#define PACKETMAX  248000

GsOT            Wot[2];
GsOT_TAG        zSortTable[2][1<<OT_LENGTH];

PACKET          gpuPacketArea[2][PACKETMAX];

volatile u_char *bb0;
volatile u_char *bb1;

u_long          PadData;

/* Functions */
void InitGraphics(void);
static u_long PadRead(void);
void RenderFrame(void);

/* main() */

int main(void)
{
    FntLoad(960, 256);
    FntOpen(100, 100, 256, 200, 0, 512);
    GetPadBuf(&bb0, &bb1);

    while(1)
    {
        printf("Press SELECT to exit\n");
        PadData = PadRead();
        if (PadData & PADselect)
            break;
        RenderFrame();
    }
    ResetGraph(3);
    return 0;
}

void InitGraphics(void)
{
    /* Initialize GPU */
    GsInitGraph(
        SCREEN_W,
        SCREEN_H,
        GsINTER | GsOFSGPU,
        0,
        0
    );

    /* Create two frame buffers */
    GsDefDispBuff(
        0, 0,
        0, SCREEN_H
    );

    GsInit3D();

    Wot[0].length = OT_LENGTH;
    Wot[1].length = OT_LENGTH;

    Wot[0].org = zSortTable[0];
    Wot[1].org = zSortTable[1];

    GsClearOt(0, 0, &Wot[0]);
    GsClearOt(0, 0, &Wot[1]);
}

void RenderFrame(void)
{
    int currentBuffer = GsGetActiveBuff();
    GsSetWorkBase((PACKET*)gpuPacketArea[currentBuffer]);
    GsClearOt(0, 0, &Wot[currentBuffer]);
    FntPrint("Hello World!\n");
    FntFlush(-1);
    VSync(0);
    GsSwapDispBuff();
    GsSortClear(0, 0, 0, &Wot[currentBuffer]);
    GsDrawOt(&Wot[currentBuffer]);
}

static u_long PadRead()
{
    return ~(
        (*(bb0 + 3)) |
        (*(bb0 + 2) << 8) |
        (*(bb1 + 3) << 16) |
        (*(bb1 + 2) << 24)
    );
    
}
