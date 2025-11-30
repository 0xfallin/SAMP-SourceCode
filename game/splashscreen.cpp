#include "../main.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "gui/gui.h"

extern CGUI *pGUI;

RwTexture *splashTexture = nullptr;

#define COLOR_WHITE				0xFFFFFFFF
#define COLOR_BLACK 			0xFF000000
#define COLOR_GRAY				0xFFA9A9A9
#define COLOR_LOADING_BLACK 	0x4D000000
#define COLOR_LOADING_WHITE 	0x4DFFFFFF
#define COLOR_YELLOW 			0xFF12F0FF
#define COLOR_ORANGE 			0xFF00A5FF
#define COLOR_GRADIENT1			0xFFB09321 //Sexy Blue
#define COLOR_GRADIENT2			0xFFEDD56D
#define COLOR_GRADIENT3			0xFF4933EB //Cherry
#define COLOR_GRADIENT4			0xFF435CF4

#define USE_SPLASH_TYPE			(0)

struct stRect
{
	int x1;	// left
	int y1;	// top
	int x2;	// right
	int y2;	// bottom
};

struct stfRect
{
	float x1;
	float y1;
	float x2;
	float y2;
};

#define MAX_SCHEMAS 3
uint32_t colors[MAX_SCHEMAS][2] =
{
	{ COLOR_BLACK,			COLOR_WHITE },
	{ COLOR_LOADING_BLACK, 	COLOR_LOADING_WHITE },
	{ COLOR_GRADIENT3, 		COLOR_GRADIENT4 },
};

void LoadSplashTexture()
{
	splashTexture = (RwTexture*)LoadTextureFromDB(cryptor::create("samp").decrypt(), cryptor::create("mylogo").decrypt());
	Log(("Splashscreen initialized."));
}

void Draw(stRect *rect, uint32_t color, uint32_t color2, RwRaster *raster = nullptr, stfRect *uv = nullptr)
{
	static RwIm2DVertex vert[4];
	const RwReal nearScreenZ = *(RwReal*)(g_libGTASA+0x9DAA60);	// CSprite2d::NearScreenZ
	const RwReal recipNearClip = *(RwReal*)(g_libGTASA+0x9DAA64);	// CSprite2d::RecipNearClip

	RwIm2DVertexSetScreenX(&vert[0], rect->x1);
	RwIm2DVertexSetScreenY(&vert[0], rect->y2);
	RwIm2DVertexSetScreenZ(&vert[0], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[0], recipNearClip);
	vert[0].emissiveColor = color;
	RwIm2DVertexSetU(&vert[0], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[0], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[1], rect->x2);
	RwIm2DVertexSetScreenY(&vert[1], rect->y2);
	RwIm2DVertexSetScreenZ(&vert[1], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[1], recipNearClip);
	vert[1].emissiveColor = color2;
	RwIm2DVertexSetU(&vert[1], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[1], uv ? uv->y2 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[2], rect->x1);
	RwIm2DVertexSetScreenY(&vert[2], rect->y1);
	RwIm2DVertexSetScreenZ(&vert[2], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[2], recipNearClip);
	vert[2].emissiveColor = color;
	RwIm2DVertexSetU(&vert[2], uv ? uv->x1 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[2], uv ? uv->y1 : 0.0f, recipNearClip);

	RwIm2DVertexSetScreenX(&vert[3], rect->x2);
	RwIm2DVertexSetScreenY(&vert[3], rect->y1);
	RwIm2DVertexSetScreenZ(&vert[3], nearScreenZ);
	RwIm2DVertexSetRecipCameraZ(&vert[3], recipNearClip);
	vert[3].emissiveColor = color2;
	RwIm2DVertexSetU(&vert[3], uv ? uv->x2 : 0.0f, recipNearClip);
	RwIm2DVertexSetV(&vert[3], uv ? uv->y1 : 0.0f, recipNearClip);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)raster);
	RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vert, 4);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);
}

float percent;
void RenderSplash()
{
	stRect rect;
	stfRect uv;
	stfRect sRect;

	// black the background
	rect.x1 = 0;
	rect.y1 = 0;
	rect.x2 = RsGlobal->maximumWidth;
	rect.y2 = RsGlobal->maximumHeight;
	Draw(&rect, colors[0][0], colors[0][0]);

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

	// texture background
	#if USE_SPLASH_TYPE == 0
	rect.x1 = RsGlobal->maximumWidth * 0.05;
	rect.y1 = RsGlobal->maximumHeight * 0.25;
	#else if USE_SPLASH_TYPE == 1
	rect.x1 = 0.0;
	rect.y1 = 0.0;
	#endif

	rect.x2 = RsGlobal->maximumWidth - rect.x1;
	rect.y2 = RsGlobal->maximumHeight - rect.y1;
	uv.x1 = 0.0f;
	uv.y1 = 0.0f;
	uv.x2 = 1.0;
	uv.y2 = 1.0;
	Draw(&rect, colors[0][1], colors[0][1], splashTexture->raster, &uv);

	// loading background
	rect.x1 = RsGlobal->maximumWidth * 0.050;
	rect.y1 = RsGlobal->maximumHeight * 0.955;
	rect.x2 = RsGlobal->maximumWidth * 0.955;
	rect.y2 = RsGlobal->maximumHeight * 0.935;
	Draw(&rect, colors[1][1], colors[1][1]);
	
	// loading foreground
	percent = *(float*)(g_libGTASA+0x8F08C0);
	if(percent <= 0.0f) return;
	float mult = percent / 100.0f;

	// offset
	rect.x2 = (int)rect.x1 + (mult * 0.88435 * RsGlobal->maximumWidth);
	Draw(&rect, colors[2][1], colors[2][0]);

	// old loading in texture
	/*const float percent = *(float*)(g_libGTASA+0x8F08C0);
	if(percent <= 0.0f) return;
	float mult = percent/100.0f;
	
	// offset
	float newX = rect.x1 + (mult * 0.9 * RsGlobal->maximumWidth);

	sRect.x1 = (float)rect.x1; 	// x1
	sRect.y2 = (float)rect.y1; 	// y1
	sRect.x2 = (float)newX;		// x2
	sRect.y1 = (float)rect.y2;	// y2
	SetScissorRect((void*)&sRect);
	Draw(&rect, colors[color_scheme][1], splashTexture->raster, &uv);
	*/

	sRect.x1 = 0.0f;
	sRect.y1 = 0.0f;
	sRect.x2 = 0.0f;
	sRect.y2 = 0.0f;
	SetScissorRect((void*)&sRect);
}

void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRenderWare_NewFrame();

void RenderSplashScreen()
{
	RenderSplash();

	if(!pGUI) return;

	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	ImVec2 _ImVec2 = ImVec2(pGUI->ScaleX(10), pGUI->ScaleY(5));
	pGUI->RenderTextSize(_ImVec2, ImColor(255, 255, 255), false, ("DEVELOPER LUFZYY"), nullptr, ImGui::GetFontSize() * 0.75);

	ImVec2 ImVec2_ = ImVec2(RsGlobal->maximumWidth * 0.050, RsGlobal->maximumHeight * 0.895);
	pGUI->RenderTextSize(ImVec2_, ImColor(255, 255, 255), true, ("DEVELOPER LUFZYY"), nullptr, ImGui::GetFontSize() * 1.25);

	char szLoadingPercent[512];
	if(percent >= 100.0)
		percent = 100.0;

	sprintf(szLoadingPercent, "%.1f%%", percent);

	ImGui::GetOverlayDrawList()->AddText(ImVec2(RsGlobal->maximumWidth * 0.48, RsGlobal->maximumHeight * 0.90), COLOR_WHITE, 
		szLoadingPercent
	);

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
}