#include <stdio.h>
#include <fstream>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "PPCD.h"

using namespace std;

const int ResX = 650, ResY = 900;
const int FPS = 60;

int BestScore = 0;

ALLEGRO_DISPLAY *Display = NULL;
ALLEGRO_EVENT_QUEUE *GameQueue = NULL;
ALLEGRO_TIMER *Timer = NULL;
ALLEGRO_EVENT GameEvent;

class playerClass {
public:
	int Score;
	int Width;
	int Height;
	int PosX;
	int PosY;
	int Speed;

	int Ammo;
	int MaxAmmo;

	float Fuel;
	float MaxFuel;

	bool Up;
	bool Down;
	bool Left;
	bool Right;

	int CurFrame;
	int FrameCount;
	int MaxFrame;
	int FrameW;
	int FrameH;
	int FrameDelay;

	ALLEGRO_BITMAP *Bitmap;
	ALLEGRO_BITMAP *MaskBitmap;

	mask_t *Mask;
};
playerClass Player;

class asteroidClass
{
public:
	int PosX;
	int PosY;
	int Speed;
	int HP;
	int ConstHP;

	int CurFrame;
	int FrameCount;
	int MaxFrame;
	int FrameW;
	int FrameH;
	int FrameDelay;

	ALLEGRO_BITMAP *Bitmap;
	ALLEGRO_BITMAP *MaskBitmap;

	mask_t *Mask;
};
asteroidClass AsteroidA;
asteroidClass AsteroidB;
asteroidClass AsteroidC;
asteroidClass AsteroidD;
asteroidClass AsteroidE;
asteroidClass AsteroidF;

class ExplosionClass {
public:
	int	CurFrame;
	int FrameCount;
	int MaxFrame;
	int FrameW;
	int FrameH;
	int FrameDelay;

	int PosX;
	int PosY;

	bool ExplosionEnd;
	bool StartAnimation;

	ALLEGRO_BITMAP *Bitmap;
};
ExplosionClass Boom;
ExplosionClass BoomA;
ExplosionClass BoomB;
ExplosionClass BoomC;
ExplosionClass BoomD;
ExplosionClass BoomE;
ExplosionClass BoomF;

class FuelClass {
public:
	int PosX;
	int PosY;
	int Width;
	int Height;
	int Speed;
	int HP;

	ALLEGRO_BITMAP *Bitmap;
	mask_t *Mask;
};
FuelClass FuelUp;

class SkullClass {
public:
	int PosX;
	int PosY;
	int Width;
	int Height;
	int Speed;
	int HP;

	ALLEGRO_BITMAP *Bitmap;
	mask_t *Mask;
};
SkullClass SkullUp;

class AmmoClass {
public:
	int PosX;
	int PosY;
	int Width;
	int Height;
	int Speed;
	int HP;

	ALLEGRO_BITMAP *Bitmap;
	mask_t *Mask;
};
AmmoClass AmmoUp;

bool InstallAllegro()
{
	bool Good = true;
	al_init_font_addon();
	al_init_ttf_addon();

	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);

	if (!al_init()) {
		al_show_native_message_box(Display, "Blad", "AL_INIT_ERROR", "App window error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		Good = false;
	}
	if (!al_install_keyboard()) {
		al_show_native_message_box(Display, "Blad", "KEYBOARD_ERROR", "Keyboard init error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		Good = false;
	}
	if (!al_install_mouse()) {
		al_show_native_message_box(Display, "Blad", "MOUSE_ERROR", "Mouse init error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		Good = false;
	}
	if (!al_init_image_addon()) {
		al_show_native_message_box(Display, "Blad", "IMAGE_ADDON_ERROR", "Bitmap init error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		Good = false;
	}
	if (!al_init_primitives_addon()) {
		al_show_native_message_box(Display, "Blad", "PRIMITIVES_ADDON_ERROR", "Primitives init error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		Good = false;
	}
	if (!al_install_audio()){
		al_show_native_message_box(Display, "Blad", "AUDIO_ADDON_ERROR", "Sound init error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	if (!al_init_acodec_addon()){
		al_show_native_message_box(Display, "Blad", "AUDIO_CODEC_ERROR", "Sound lib error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	if (!al_reserve_samples(20)){
		al_show_native_message_box(Display, "Blad", "AUDIO_SAMPLE_ERROR", "Sample init error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	Display = al_create_display(ResX, ResY);
	if (!Display) {
		al_show_native_message_box(Display, "Blad", "DISPLAY_ERROR", "Screen init error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		Good = false;
	}
	Timer = al_create_timer(1.0 / FPS);
	if (!Timer) {
		al_show_native_message_box(Display, "Blad", "TIMER_ERROR", "Timer init error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(Display);
		Good = false;
	}
	return Good;
}

int LoadHighScore(int &BestScore)
{
	string temp = "";
	fstream plik;
	plik.open("Files/Avoid", ios::in);
	if (plik)
	{
		plik >> temp;
		if (!temp.empty())
			BestScore = stoi(temp);
		plik.close();
	}
	return BestScore;
}
void SaveHighScore(int points, int &BestScore)
{
	fstream plik;
	plik.open("Files/Avoid", ios::out);
	plik << points;
	plik.close();

	BestScore = points;
}

int Menu(ALLEGRO_FONT *Font, ALLEGRO_FONT *SmallFont, int HighScore, int LastScore) {
	ALLEGRO_BITMAP *MenuBackground1 = NULL;
	ALLEGRO_BITMAP *MenuBackground2 = NULL;

	MenuBackground1 = al_load_bitmap("Files/img/background.png");
	MenuBackground2 = al_load_bitmap("Files/img/background.png");

	int BackgroundHeight = al_get_bitmap_height(MenuBackground1);
	int BackgroundPos1 = BackgroundHeight*(-1) + ResY;
	int BackgroundPos2 = BackgroundHeight*(-1) + ResY;

	int MenuChoice = 0;

	bool Stop = false;

	ALLEGRO_EVENT_QUEUE *MenuQueue = NULL;

	MenuQueue = al_create_event_queue();
	al_register_event_source(MenuQueue, al_get_display_event_source(Display));
	al_register_event_source(MenuQueue, al_get_timer_event_source(Timer));
	al_register_event_source(MenuQueue, al_get_keyboard_event_source());

	while (Stop == false) {
		ALLEGRO_EVENT MenuEvent;

		al_wait_for_event(MenuQueue, &MenuEvent);

		if (MenuEvent.type == ALLEGRO_EVENT_TIMER) {
			BackgroundPos1++;
			BackgroundPos2++;
			if (BackgroundPos1 > ResY)
				BackgroundPos1 = BackgroundHeight*(-1) + ResY;
			if(BackgroundPos2 > ResY)
				BackgroundPos2 = BackgroundHeight*(-1) + ResY;
		}
		if (MenuEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			MenuChoice = 2;
			Stop = true;
		}
		if (MenuEvent.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (MenuEvent.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				MenuChoice--;
				break;
			case ALLEGRO_KEY_DOWN:
				MenuChoice++;
				break;
			case ALLEGRO_KEY_ENTER:
				Stop = true;
				break;
			case ALLEGRO_KEY_ESCAPE:
				MenuChoice = 2;
				Stop = true;
				break;
			}
			if (MenuChoice < 0)
				MenuChoice = 0;
			else if (MenuChoice > 2)
				MenuChoice = 2;
		}
		if (al_is_event_queue_empty(MenuQueue)) {
			al_draw_bitmap(MenuBackground1, 0, BackgroundPos1, 0);
			al_draw_bitmap(MenuBackground2, 0, BackgroundPos2 - BackgroundHeight, 0);

			al_draw_textf(Font, al_map_rgb(255, 255, 255), ResX / 2, 30, ALLEGRO_ALIGN_CENTRE, "High Score: %i", LoadHighScore(BestScore));
			al_draw_textf(SmallFont, al_map_rgb(255, 255, 255), ResX / 2, 70, ALLEGRO_ALIGN_CENTRE, "Last Run: %i", Player.Score);

			if (MenuChoice == 0){
				al_draw_textf(Font, al_map_rgb(255, 130, 40), ResX / 2, 320, ALLEGRO_ALIGN_CENTRE, "New Game");
				al_draw_textf(Font, al_map_rgb(255, 255, 255), ResX / 2, 400, ALLEGRO_ALIGN_CENTRE, "Music");
				al_draw_textf(Font, al_map_rgb(255, 255, 255), ResX / 2, 480, ALLEGRO_ALIGN_CENTRE, "Exit");
			}
			if (MenuChoice == 1){
				al_draw_textf(Font, al_map_rgb(255, 255, 255), ResX / 2, 320, ALLEGRO_ALIGN_CENTRE, "New Game");
				al_draw_textf(Font, al_map_rgb(255, 130, 40), ResX / 2, 400, ALLEGRO_ALIGN_CENTRE, "Music");;
				al_draw_textf(Font, al_map_rgb(255, 255, 255), ResX / 2, 480, ALLEGRO_ALIGN_CENTRE, "Exit");
			}
			if (MenuChoice == 2){
				al_draw_textf(Font, al_map_rgb(255, 255, 255), ResX / 2, 320, ALLEGRO_ALIGN_CENTRE, "New Game");
				al_draw_textf(Font, al_map_rgb(255, 255, 255), ResX / 2, 400, ALLEGRO_ALIGN_CENTRE, "Music");
				al_draw_textf(Font, al_map_rgb(255, 130, 40), ResX / 2, 480, ALLEGRO_ALIGN_CENTRE, "Exit");
			}
			al_flip_display();
		}
	}
	al_destroy_bitmap(MenuBackground1);
	al_destroy_bitmap(MenuBackground2);
	al_destroy_event_queue(MenuQueue);
	return MenuChoice;
}

void LoadGame(bool &FirstRun, bool &Collision) {
	if (FirstRun == true) {
		Player.Bitmap = al_load_bitmap("Files/img/player.png");
		Player.MaskBitmap = al_load_bitmap("Files/img/playerCol.png");
		al_convert_mask_to_alpha(Player.MaskBitmap, al_map_rgb(255, 255, 255));
		Player.Mask = Mask_New(Player.MaskBitmap);

		Boom.Bitmap = al_load_bitmap("Files/img/explosion.png");

		FuelUp.Bitmap = al_load_bitmap("Files/img/fuel.png");
		al_convert_mask_to_alpha(FuelUp.Bitmap, al_map_rgb(255, 255, 255));
		FuelUp.Mask = Mask_New(FuelUp.Bitmap);

		AmmoUp.Bitmap = al_load_bitmap("Files/img/ammo.png");
		al_convert_mask_to_alpha(AmmoUp.Bitmap, al_map_rgb(255, 255, 255));
		AmmoUp.Mask = Mask_New(AmmoUp.Bitmap);

		SkullUp.Bitmap = al_load_bitmap("Files/img/skull.png");
		al_convert_mask_to_alpha(SkullUp.Bitmap, al_map_rgb(255, 255, 255));
		SkullUp.Mask = Mask_New(SkullUp.Bitmap);

		AsteroidA.Bitmap = al_load_bitmap("Files/img/asteroidA.png");
		AsteroidB.Bitmap = al_load_bitmap("Files/img/asteroidB.png");
		AsteroidC.Bitmap = al_load_bitmap("Files/img/asteroidC.png");
		AsteroidD.Bitmap = al_load_bitmap("Files/img/asteroidD.png");
		AsteroidE.Bitmap = al_load_bitmap("Files/img/asteroidE.png");
		AsteroidF.Bitmap = al_load_bitmap("Files/img/asteroidF.png");

		AsteroidA.MaskBitmap = al_load_bitmap("Files/img/asteroidCol.png");
		al_convert_mask_to_alpha(AsteroidA.MaskBitmap, al_map_rgb(255, 255, 255));

		AsteroidA.Mask = AsteroidB.Mask = AsteroidC.Mask = AsteroidD.Mask = AsteroidE.Mask = AsteroidF.Mask = Mask_New(AsteroidA.MaskBitmap);

		al_convert_mask_to_alpha(AsteroidA.Bitmap, al_map_rgb(255, 255, 255));
		al_convert_mask_to_alpha(AsteroidB.Bitmap, al_map_rgb(255, 255, 255));
		al_convert_mask_to_alpha(AsteroidC.Bitmap, al_map_rgb(255, 255, 255));
		al_convert_mask_to_alpha(AsteroidD.Bitmap, al_map_rgb(255, 255, 255));
		al_convert_mask_to_alpha(AsteroidE.Bitmap, al_map_rgb(255, 255, 255));
		al_convert_mask_to_alpha(AsteroidF.Bitmap, al_map_rgb(255, 255, 255));
		al_convert_mask_to_alpha(Player.Bitmap, al_map_rgb(255, 255, 255));
		al_convert_mask_to_alpha(Boom.Bitmap, al_map_rgb(255, 255, 255));

		al_lock_bitmap(FuelUp.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
		al_lock_bitmap(AmmoUp.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
		al_lock_bitmap(SkullUp.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
		al_lock_bitmap(AsteroidA.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
		al_lock_bitmap(AsteroidB.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
		al_lock_bitmap(AsteroidC.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
		al_lock_bitmap(AsteroidD.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
		al_lock_bitmap(AsteroidE.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
		al_lock_bitmap(AsteroidF.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
		al_lock_bitmap(Player.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
		al_lock_bitmap(Boom.Bitmap, 0, ALLEGRO_LOCK_READWRITE);
	}

	GameQueue = al_create_event_queue();
	al_register_event_source(GameQueue, al_get_display_event_source(Display));
	al_register_event_source(GameQueue, al_get_timer_event_source(Timer));
	al_register_event_source(GameQueue, al_get_keyboard_event_source());
	al_register_event_source(GameQueue, al_get_mouse_event_source());

	SkullUp.Width = al_get_bitmap_width(SkullUp.Bitmap);
	SkullUp.Height = al_get_bitmap_width(SkullUp.Bitmap);
	SkullUp.Speed = rand()%7 + 3;
	SkullUp.PosX = rand() % ResX - SkullUp.Width;
	SkullUp.PosY = (rand() % 500)*(-1);
	SkullUp.HP = 1;

	FuelUp.Width = al_get_bitmap_width(FuelUp.Bitmap);
	FuelUp.Height = al_get_bitmap_width(FuelUp.Bitmap);
	FuelUp.Speed = rand() % 7 + 3;
	FuelUp.PosX = rand() % ResX - FuelUp.Width;
	FuelUp.PosY = (rand() % 1500)*(-1);
	FuelUp.HP = 1;

	AmmoUp.Width = al_get_bitmap_width(AmmoUp.Bitmap);
	AmmoUp.Height = al_get_bitmap_width(AmmoUp.Bitmap);
	AmmoUp.Speed = rand() % 7 + 3;
	AmmoUp.PosX = rand() % ResX - AmmoUp.Width;
	AmmoUp.PosY = (rand() % 1500)*(-1);
	AmmoUp.HP = 1;

	Player.Width = al_get_bitmap_width(Player.Bitmap);
	Player.Height = al_get_bitmap_height(Player.Bitmap);
	Player.PosX = ResX / 2;
	Player.PosY = (ResY*0.95) - (Player.Height / 2);
	Player.Speed = 7;
	Player.Up = false;
	Player.Down = false;
	Player.Left = false;
	Player.Right = false;
	Player.Score = 0;
	Player.MaxFuel = Player.Fuel = 100;
	Player.MaxAmmo = Player.Ammo = 100;

	Player.CurFrame = 0;
	Player.FrameCount = 0;
	Player.FrameH = Player.FrameW = al_get_bitmap_height(Player.Bitmap);
	Player.MaxFrame = 5;
	Player.FrameDelay = 4;

	AsteroidA.CurFrame = 0;
	AsteroidA.FrameCount = 0;
	AsteroidA.FrameH = AsteroidA.FrameW = al_get_bitmap_height(AsteroidA.Bitmap);
	AsteroidA.MaxFrame = 16;
	AsteroidA.FrameDelay = 3;
	AsteroidA.Speed = 2;
	AsteroidA.ConstHP = AsteroidA.HP = 3;

	AsteroidB.CurFrame = 0;
	AsteroidB.FrameCount = 0;
	AsteroidB.FrameH = AsteroidB.FrameW = al_get_bitmap_height(AsteroidB.Bitmap);
	AsteroidB.MaxFrame = 16;
	AsteroidB.FrameDelay = 3;
	AsteroidB.Speed = 3;
	AsteroidB.ConstHP = AsteroidB.HP = 3;

	AsteroidC.CurFrame = 0;
	AsteroidC.FrameCount = 0;
	AsteroidC.FrameH = AsteroidC.FrameW = al_get_bitmap_height(AsteroidC.Bitmap);
	AsteroidC.MaxFrame = 16;
	AsteroidC.FrameDelay = 3;
	AsteroidC.Speed = 4;
	AsteroidC.ConstHP = AsteroidC.HP = 3;

	AsteroidD.CurFrame = 0;
	AsteroidD.FrameCount = 0;
	AsteroidD.FrameH = AsteroidD.FrameW = al_get_bitmap_height(AsteroidD.Bitmap);
	AsteroidD.MaxFrame = 16;
	AsteroidD.FrameDelay = 3;
	AsteroidD.Speed = 5;
	AsteroidD.ConstHP = AsteroidD.HP = 3;

	AsteroidE.CurFrame = 0;
	AsteroidE.FrameCount = 0;
	AsteroidE.FrameH = AsteroidE.FrameW = al_get_bitmap_height(AsteroidE.Bitmap);
	AsteroidE.MaxFrame = 16;
	AsteroidE.FrameDelay = 3;
	AsteroidE.Speed = 6;
	AsteroidE.ConstHP = AsteroidE.HP = 3;

	AsteroidF.CurFrame = 0;
	AsteroidF.FrameCount = 0;
	AsteroidF.FrameH = AsteroidF.FrameW = al_get_bitmap_height(AsteroidF.Bitmap);
	AsteroidF.MaxFrame = 16;
	AsteroidF.FrameDelay = 3;
	AsteroidF.Speed = 7;
	AsteroidF.ConstHP = AsteroidF.HP = 3;

	Boom.CurFrame = 0;
	Boom.FrameCount = 0;
	Boom.FrameH = al_get_bitmap_height(Boom.Bitmap);
	Boom.FrameW = Boom.FrameH + 5;
	Boom.MaxFrame = 13;
	Boom.FrameDelay = 7;
	Boom.ExplosionEnd = false;
	Boom.StartAnimation = false;

	AsteroidA.PosX = rand() % (ResX - AsteroidA.FrameW);
	AsteroidA.PosY = 0;

	AsteroidB.PosX = rand() % (ResX - AsteroidB.FrameW);
	AsteroidB.PosY = -50;

	AsteroidC.PosX = rand() % (ResX - AsteroidC.FrameW);
	AsteroidC.PosY = -100;

	AsteroidD.PosX = rand() % (ResX - AsteroidD.FrameW);
	AsteroidD.PosY = -150;

	AsteroidE.PosX = rand() % (ResX - AsteroidE.FrameW);
	AsteroidE.PosY = -200;

	AsteroidF.PosX = rand() % (ResX - AsteroidF.FrameW);
	AsteroidF.PosY = -250;

	Collision = false;
}

void PlayerMovement(bool Left, bool Right, bool Up, bool Down,int &PlayerPosX, int &PlayerPosY,int PlayerSpeed) {
	if (Left == true) {
		PlayerPosX -= PlayerSpeed;
	}
	if (Right == true) {
		PlayerPosX += PlayerSpeed;
	}
	if (Up == true) {
		PlayerPosY -= PlayerSpeed;
	}
	if (Down == true) {
		PlayerPosY += PlayerSpeed;
	}
}

void Collisions(int &PosX, int &PosY, int ShipWidth, int ShipHeight, int Speed) {
	if (PosX - ShipWidth/2 < 0)
		PosX += Speed;
	else if (PosX + ShipWidth/2 > ResX)
		PosX -= Speed;

	if (PosY + ShipHeight / 2 / 2 > ResY)
		PosY -= Speed;
	else if (PosY < ResY / 3)
		PosY += Speed;
}

void Animation(int &FrameCount, int FrameDelay, int &CurrentFrame, int MaxFrame) {
	if (++FrameCount >= FrameDelay)
	{
		if (++CurrentFrame >= MaxFrame)
			CurrentFrame = 0;

		FrameCount = 0;
	}
}
void Movement(int &PosX, int &PosY, int Speed, int range, int FrameWidth, int &ConstHP, int &HP) {
	PosY = PosY + Speed;
	if (PosY > ResY){
		PosY = (rand() % range)*-1;
		PosX = rand() % (ResX - 2 * FrameWidth) + FrameWidth;
		ConstHP = HP = (rand() %10) + 3;
	}
}
void ExplosionAnimation(int &FrameCount, int FrameDelay, int &CurrentFrame, int MaxFrame, bool &StartExplosion, bool &EndGame)
{
	if (++FrameCount >= FrameDelay)
	{
		if (++CurrentFrame >= MaxFrame)
			CurrentFrame = 0;

		FrameCount = 0;
	}
	if (CurrentFrame == 12) {
		StartExplosion = false;
		EndGame = true;
	}
}

void PowerUpMovement(int &PosX, int &PosY, int Speed, int range, int FrameWidth, int ConstHP, int &HP) {
	PosY = PosY + Speed;
	if (PosY > ResY) {
		PosY = (rand() % (range+1))*-1;
		PosX = rand() % (ResX - 2 * FrameWidth) + FrameWidth;
		ConstHP = HP = 1;
	}
}

bool IfHit(int ShootX, int ShootY, int ShootW, int ShootH, int EnemyX, int EnemyY, int EnemyW, int EnemyH, int HP) {
	bool Hit = true;

	if ((ShootX-ShootW/2 > EnemyX-EnemyW/2 + EnemyW - 1) || (ShootY-ShootH/2 > EnemyY-EnemyH/2 + EnemyH - 1) || (EnemyX-EnemyW/2 > ShootX-ShootW/2 + ShootW - 1) || (EnemyY/EnemyH/2 > ShootY/ShootH/2 + ShootH - 1)) {
		Hit = false;
	}
	return Hit;
}

void DrawHUD(float Fuel, float MaxFuel, int Ammo, int MaxAmmo, ALLEGRO_FONT *Font) {
	int Max = ResY - 147;
	int Min = ResY - 50;
	int Fcur = (Min - Max)*Fuel / MaxFuel;
	int Acur = (Min - Max)*Ammo / MaxAmmo;

	al_draw_rectangle(50, ResY-47, 75, ResY - 150, al_map_rgb(240, 30, 40), 6);
	al_draw_filled_rectangle(53, Min, 72, Min-Fcur, al_map_rgb(255, 100, 30));
	al_draw_textf(Font, al_map_rgb(255, 255, 255), 62, ResY-40, ALLEGRO_ALIGN_CENTRE, "FUEL");

	al_draw_rectangle(ResX-75, ResY - 47, ResX-50, ResY - 150, al_map_rgb(0,0,255), 6);
	al_draw_filled_rectangle(ResX-72, Min, ResX-53, Min - Acur, al_map_rgb(0,128,255));
	al_draw_textf(Font, al_map_rgb(255, 255, 255), ResX-62, ResY - 40, ALLEGRO_ALIGN_CENTRE, "AMMO");
}

int main() {
	srand(time(NULL));

	InstallAllegro();

	ALLEGRO_FONT *Font = al_load_font("Files/SHOWG.ttf", 28, 0);
	ALLEGRO_FONT *SmallFont = al_load_ttf_font("Files/SHOWG.ttf", 20, 0);
	ALLEGRO_SAMPLE *Explosion = NULL;
	ALLEGRO_SAMPLE *Music = NULL;
	ALLEGRO_SAMPLE *LaserShoot = NULL;

	bool PlayMusic = true;
	bool NoSound = false;
	bool End = false;

	int HighScore = 0;
	int LastScore = 0;

	al_start_timer(Timer);

	Explosion = al_load_sample("Files/sounds/explosion.wav");
	Music = al_load_sample("Files/sounds/MainMusic.wav");
	LaserShoot = al_load_sample("Files/sounds/laser.wav");
	if (!Explosion || !Music || !LaserShoot){
		al_show_native_message_box(Display, "Blad", "MUSIC_LOAD_ERROR", "Sound files missing", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		PlayMusic = false;
		NoSound = true;
	}

	if (PlayMusic == true)
		al_play_sample(Music, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
	while (End == false) {
		int MenuChoice = Menu(Font, SmallFont, HighScore, LastScore);
		if (MenuChoice == 0) {
			bool FirstRun = true;
			bool Collision = false;
			bool PlayerCollision = false;
			bool EndGame = false;
			bool ExplosionSound = false;

			ALLEGRO_BITMAP *FUEL = NULL;
			ALLEGRO_BITMAP *Ammo = NULL;
			ALLEGRO_BITMAP *Skull = NULL;
			ALLEGRO_BITMAP *BackgroundBitmap1 = NULL;
			ALLEGRO_BITMAP *BackgroundBitmap2 = NULL;
			ALLEGRO_BITMAP *Bullet = NULL;
			ALLEGRO_BITMAP *BulletCol = NULL;
			
			BackgroundBitmap1 = al_load_bitmap("Files/img/background.png");
			BackgroundBitmap2 = al_load_bitmap("Files/img/background.png");
			Bullet = al_load_bitmap("Files/img/bullet.png");
			BulletCol = al_load_bitmap("Files/img/bulletCol.png");

			al_convert_mask_to_alpha(Bullet, al_map_rgb(255, 255, 255));
			al_convert_mask_to_alpha(BulletCol, al_map_rgb(255, 255, 255));

			mask_t *BulletMask = NULL;

			BulletMask = Mask_New(BulletCol);

			al_lock_bitmap(BackgroundBitmap1, 0, ALLEGRO_LOCK_READWRITE);
			al_lock_bitmap(BackgroundBitmap2, 0, ALLEGRO_LOCK_READWRITE);

			LoadGame(FirstRun, Collision);

			int BackgroundHeight = al_get_bitmap_height(BackgroundBitmap1);
			int BackgroundPosition1 = BackgroundHeight*(-1) + ResY;
			int BackgroundPosition2 = BackgroundHeight*(-1) + ResY;

			float ShootsX[100];
			float ShootsY[100];

			int ShootCounter = 0;
			int MinShoot = 0;
			int ShootSpeed = 10;
			bool IfShoot = false;

			bool ReverseSteering = false;

			int BulletH = al_get_bitmap_height(Bullet);
			int BulletW = al_get_bitmap_width(Bullet);

			while (EndGame == false) {
				al_wait_for_event(GameQueue, &GameEvent);

				if (GameEvent.type == ALLEGRO_EVENT_TIMER) {
					Animation(AsteroidA.FrameCount, AsteroidA.FrameDelay, AsteroidA.CurFrame, AsteroidA.MaxFrame);
					Animation(AsteroidB.FrameCount, AsteroidB.FrameDelay, AsteroidB.CurFrame, AsteroidB.MaxFrame);
					Animation(AsteroidC.FrameCount, AsteroidC.FrameDelay, AsteroidC.CurFrame, AsteroidC.MaxFrame);
					Animation(AsteroidD.FrameCount, AsteroidD.FrameDelay, AsteroidD.CurFrame, AsteroidD.MaxFrame);
					Animation(AsteroidE.FrameCount, AsteroidE.FrameDelay, AsteroidE.CurFrame, AsteroidE.MaxFrame);
					Animation(AsteroidF.FrameCount, AsteroidF.FrameDelay, AsteroidF.CurFrame, AsteroidF.MaxFrame);

					Movement(AsteroidA.PosX, AsteroidA.PosY, AsteroidA.Speed, 200, AsteroidA.FrameW,AsteroidA.ConstHP,AsteroidA.HP);
					Movement(AsteroidB.PosX, AsteroidB.PosY, AsteroidB.Speed, 200, AsteroidB.FrameW, AsteroidB.ConstHP, AsteroidB.HP);
					Movement(AsteroidC.PosX, AsteroidC.PosY, AsteroidC.Speed, 200, AsteroidC.FrameW, AsteroidC.ConstHP, AsteroidC.HP);
					Movement(AsteroidD.PosX, AsteroidD.PosY, AsteroidD.Speed, 200, AsteroidD.FrameW, AsteroidD.ConstHP, AsteroidD.HP);
					Movement(AsteroidE.PosX, AsteroidE.PosY, AsteroidE.Speed, 200, AsteroidE.FrameW, AsteroidE.ConstHP, AsteroidE.HP);
					Movement(AsteroidF.PosX, AsteroidF.PosY, AsteroidF.Speed, 200, AsteroidF.FrameW, AsteroidF.ConstHP, AsteroidF.HP);

					PowerUpMovement(FuelUp.PosX, FuelUp.PosY, FuelUp.Speed, 15000, FuelUp.Width, 1, FuelUp.HP);
					PowerUpMovement(AmmoUp.PosX, AmmoUp.PosY, AmmoUp.Speed, 13000, AmmoUp.Width, 1, AmmoUp.HP);
					PowerUpMovement(SkullUp.PosX, SkullUp.PosY, SkullUp.Speed, 1000, SkullUp.Width, 1, SkullUp.HP);

					Animation(Player.FrameCount, Player.FrameDelay, Player.CurFrame, Player.MaxFrame);

					if (Mask_Collide(AsteroidA.Mask, Player.Mask, AsteroidA.PosX - Player.PosX, AsteroidA.PosY - Player.PosY) ||
						Mask_Collide(AsteroidB.Mask, Player.Mask, AsteroidB.PosX - Player.PosX, AsteroidB.PosY - Player.PosY) ||
						Mask_Collide(AsteroidC.Mask, Player.Mask, AsteroidC.PosX - Player.PosX, AsteroidC.PosY - Player.PosY) ||
						Mask_Collide(AsteroidD.Mask, Player.Mask, AsteroidD.PosX - Player.PosX, AsteroidD.PosY - Player.PosY) ||
						Mask_Collide(AsteroidE.Mask, Player.Mask, AsteroidE.PosX - Player.PosX, AsteroidE.PosY - Player.PosY) ||
						Mask_Collide(AsteroidF.Mask, Player.Mask, AsteroidF.PosX - Player.PosX, AsteroidF.PosY - Player.PosY))
						PlayerCollision = true;
					else
						PlayerCollision = false;

					if (PlayerCollision == true && ExplosionSound == false) {
						ExplosionSound = true;
						al_play_sample(Explosion, 0.9, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					}

					if (Mask_Collide(Player.Mask, FuelUp.Mask, Player.PosX - FuelUp.PosX, Player.PosY - FuelUp.PosY)) {
						FuelUp.HP--;
						Player.Fuel = Player.MaxFuel;
					}
					if (Mask_Collide(Player.Mask, AmmoUp.Mask, Player.PosX - AmmoUp.PosX, Player.PosY - AmmoUp.PosY)) {
						AmmoUp.HP--;
						Player.Ammo = Player.MaxAmmo;
					}
					if (Mask_Collide(Player.Mask, SkullUp.Mask, Player.PosX - SkullUp.PosX, Player.PosY - SkullUp.PosY)) {
						SkullUp.HP--;
						Player.Up = false;
						Player.Down = false;
						Player.Left = false;
						Player.Right = false;
						if (ReverseSteering == false)
							ReverseSteering = true;
						else
							ReverseSteering = false;
					}

					BackgroundPosition1++;
					BackgroundPosition2++;
					if (BackgroundPosition1 > ResY)
						BackgroundPosition1 = BackgroundHeight*(-1) + ResY;
					if (BackgroundPosition2 > ResY)
						BackgroundPosition2 = BackgroundHeight*(-1) + ResY;

					if (ShootCounter > 100) {
						ShootCounter = 0;
						MinShoot = 0;
					}
					for (int i = MinShoot;i < ShootCounter;i++) {
						ShootsY[i] -= ShootSpeed;
						if(ShootsX[i])
						if (ShootsY[i] < 0)
							MinShoot++;
						if (ShootsX[i] < 0 || ShootsX[i] > ResX)
							MinShoot++;

						if (Mask_Collide(BulletMask, AsteroidA.Mask, ShootsX[i] - AsteroidA.PosX,ShootsY[i]- AsteroidA.PosY - AsteroidA.Speed - ShootSpeed)) {
							AsteroidA.HP--;
							MinShoot++;
						}
						if (Mask_Collide(BulletMask, AsteroidB.Mask, ShootsX[i] - AsteroidB.PosX, ShootsY[i] - AsteroidB.PosY - AsteroidB.Speed - ShootSpeed)) {
							AsteroidB.HP--;
							MinShoot++;
						}
						if (Mask_Collide(BulletMask, AsteroidC.Mask, ShootsX[i] - AsteroidC.PosX, ShootsY[i] - AsteroidC.PosY - AsteroidC.Speed - ShootSpeed)) {
							AsteroidC.HP--;
							MinShoot++;
						}
						if (Mask_Collide(BulletMask, AsteroidD.Mask, ShootsX[i] - AsteroidD.PosX, ShootsY[i] - AsteroidD.PosY - AsteroidD.Speed - ShootSpeed)) {
							AsteroidD.HP--;
							MinShoot++;
						}
						if (Mask_Collide(BulletMask, AsteroidE.Mask, ShootsX[i] - AsteroidE.PosX, ShootsY[i] - AsteroidE.PosY - AsteroidE.Speed - ShootSpeed)) {
							AsteroidE.HP--;
							MinShoot++;
						}
						if (Mask_Collide(BulletMask, AsteroidF.Mask, ShootsX[i] - AsteroidF.PosX, ShootsY[i] - AsteroidF.PosY - AsteroidF.Speed - ShootSpeed)) {
							AsteroidF.HP--;
							MinShoot++;
						}
					}
					Player.Fuel -= (float)Player.Speed/100;
					if (Player.Fuel < 0)
						Player.Fuel = 0;
					PlayerMovement(Player.Left, Player.Right, Player.Up, Player.Down, Player.PosX, Player.PosY, Player.Speed);
					Collisions(Player.PosX, Player.PosY, Player.FrameW, Player.Height, Player.Speed);
				}
				if (GameEvent.type == ALLEGRO_EVENT_KEY_DOWN && Boom.StartAnimation == false && Player.Fuel > 0) {
					if (ReverseSteering == false) {
						switch (GameEvent.keyboard.keycode)
						{
						case ALLEGRO_KEY_RIGHT:
							Player.Right = true;
							break;
						case ALLEGRO_KEY_LEFT:
							Player.Left = true;
							break;
						case ALLEGRO_KEY_UP:
							Player.Up = true;
							break;
						case ALLEGRO_KEY_DOWN:
							Player.Down = true;
							break;
						case ALLEGRO_KEY_ESCAPE:
							EndGame = true;
							break;
						case ALLEGRO_KEY_SPACE:
							if (Player.Ammo > 0) {
								ShootsX[ShootCounter] = Player.PosX;
								ShootsY[ShootCounter] = Player.PosY;
								ShootCounter++;
								Player.Ammo--;
								if (PlayMusic == true)
									al_play_sample(LaserShoot, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
							}
							break;
						}
					}
					else{
						switch (GameEvent.keyboard.keycode)
						{
						case ALLEGRO_KEY_RIGHT:
							Player.Left = true;
							break;
						case ALLEGRO_KEY_LEFT:
							Player.Right = true;
							break;
						case ALLEGRO_KEY_UP:
							Player.Down = true;
							break;
						case ALLEGRO_KEY_DOWN:
							Player.Up = true;
							break;
						case ALLEGRO_KEY_ESCAPE:
							EndGame = true;
							break;
						case ALLEGRO_KEY_SPACE:
							if (Player.Ammo > 0) {
								ShootsX[ShootCounter] = Player.PosX;
								ShootsY[ShootCounter] = Player.PosY;
								ShootCounter++;
								Player.Ammo--;
								if (PlayMusic == true)
									al_play_sample(LaserShoot, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
							}
							break;
						}
					}
				}
				if (GameEvent.type == ALLEGRO_EVENT_KEY_DOWN && Boom.StartAnimation == false && Player.Fuel <= 0) {
					switch (GameEvent.keyboard.keycode)
					{
					case ALLEGRO_KEY_SPACE:
						if (Player.Ammo > 0) {
							ShootsX[ShootCounter] = Player.PosX;
							ShootsY[ShootCounter] = Player.PosY;
							ShootCounter++;
							Player.Ammo--;
							if (PlayMusic == true)
								al_play_sample(LaserShoot, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						}
						break;
					}
				}
				if (GameEvent.type == ALLEGRO_EVENT_KEY_UP || Player.Fuel <=0) {
					if (ReverseSteering == false) {
						switch (GameEvent.keyboard.keycode)
						{
						case ALLEGRO_KEY_RIGHT:
							Player.Right = false;
							break;
						case ALLEGRO_KEY_LEFT:
							Player.Left = false;
							break;
						case ALLEGRO_KEY_UP:
							Player.Up = false;
							break;
						case ALLEGRO_KEY_DOWN:
							Player.Down = false;
							break;
						}
					}
					else{
						switch (GameEvent.keyboard.keycode)
						{
						case ALLEGRO_KEY_RIGHT:
							Player.Left = false;
							break;
						case ALLEGRO_KEY_LEFT:
							Player.Right = false;
							break;
						case ALLEGRO_KEY_UP:
							Player.Down = false;
							break;
						case ALLEGRO_KEY_DOWN:
							Player.Up = false;
							break;
						}
					}
				}
				if (GameEvent.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
					IfShoot = false;
				}
				if (GameEvent.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && Boom.StartAnimation == false && Player.Ammo > 0) {
					ShootsX[ShootCounter] = Player.PosX;
					ShootsY[ShootCounter] = Player.PosY;
					ShootCounter++;
					Player.Ammo--;
					if (Player.Ammo > 0 && PlayMusic == true)
						al_play_sample(LaserShoot, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				if (al_is_event_queue_empty(GameQueue)) {
					al_draw_bitmap(BackgroundBitmap1, 0, BackgroundPosition1, 0);
					al_draw_bitmap(BackgroundBitmap2, 0, BackgroundPosition2 - BackgroundHeight, 0);
					if(AsteroidA.HP>0)
						al_draw_bitmap_region(AsteroidA.Bitmap, AsteroidA.CurFrame*AsteroidA.FrameW, 0, AsteroidA.FrameW, AsteroidA.FrameH, AsteroidA.PosX - AsteroidA.FrameW / 2, AsteroidA.PosY - AsteroidA.FrameH / 2, 0);
					else {
						if(PlayMusic == true)
							al_play_sample(Explosion, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						Player.Score += AsteroidA.ConstHP;
						AsteroidA.ConstHP = AsteroidA.HP = (rand()%10) +3;
						AsteroidA.PosY = (rand() % 50)*(-1);
						AsteroidA.PosX = rand() % (ResX - 10) + 10;
					}
					if (AsteroidB.HP>0)
						al_draw_bitmap_region(AsteroidB.Bitmap, AsteroidA.CurFrame*AsteroidB.FrameW, 0, AsteroidB.FrameW, AsteroidB.FrameH, AsteroidB.PosX - AsteroidB.FrameW / 2, AsteroidB.PosY - AsteroidB.FrameH / 2, 0);
					else {
						if (PlayMusic == true)
							al_play_sample(Explosion, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						Player.Score += AsteroidB.ConstHP;
						AsteroidB.ConstHP = AsteroidB.HP = (rand() %10) +3;
						AsteroidB.PosY = (rand() % 50)*(-1);
						AsteroidB.PosX = rand() % (ResX - 10) + 10;
					}
					if (AsteroidC.HP>0)
						al_draw_bitmap_region(AsteroidC.Bitmap, AsteroidC.CurFrame*AsteroidC.FrameW, 0, AsteroidC.FrameW, AsteroidC.FrameH, AsteroidC.PosX - AsteroidC.FrameW / 2, AsteroidC.PosY - AsteroidC.FrameH / 2, 0);
					else {
						if (PlayMusic == true)
							al_play_sample(Explosion, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						Player.Score += AsteroidC.ConstHP;
						AsteroidC.ConstHP = AsteroidC.HP = (rand() % 10) +3;
						AsteroidC.PosY = (rand() % 50)*(-1);
						AsteroidC.PosX = rand() % (ResX - 10) + 10;
					}
					if (AsteroidD.HP>0)
						al_draw_bitmap_region(AsteroidD.Bitmap, AsteroidD.CurFrame*AsteroidD.FrameW, 0, AsteroidD.FrameW, AsteroidD.FrameH, AsteroidD.PosX - AsteroidD.FrameW / 2, AsteroidD.PosY - AsteroidD.FrameH / 2, 0);
					else {
						if (PlayMusic == true)
							al_play_sample(Explosion, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						Player.Score += AsteroidD.ConstHP;
						AsteroidD.ConstHP = AsteroidD.HP = (rand() %10) +3;
						AsteroidD.PosY = (rand() % 50)*(-1);
						AsteroidD.PosX = rand() % (ResX - 10) + 10;
					}
					if (AsteroidE.HP>0)
						al_draw_bitmap_region(AsteroidE.Bitmap, AsteroidE.CurFrame*AsteroidE.FrameW, 0, AsteroidE.FrameW, AsteroidE.FrameH, AsteroidE.PosX - AsteroidE.FrameW / 2, AsteroidE.PosY - AsteroidE.FrameH / 2, 0);
					else {
						if (PlayMusic == true)
							al_play_sample(Explosion, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						Player.Score += AsteroidE.ConstHP;
						AsteroidE.ConstHP = AsteroidE.HP = (rand() %10) +3;
						AsteroidE.PosY = (rand() % 50)*(-1);
						AsteroidE.PosX = rand() % (ResX - 10) + 10;
					}
					if (AsteroidF.HP>0)
						al_draw_bitmap_region(AsteroidF.Bitmap, AsteroidF.CurFrame*AsteroidF.FrameW, 0, AsteroidF.FrameW, AsteroidF.FrameH, AsteroidF.PosX - AsteroidF.FrameW / 2, AsteroidF.PosY - AsteroidF.FrameH / 2, 0);
					else {
						if (PlayMusic == true)
							al_play_sample(Explosion, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						Player.Score += AsteroidF.ConstHP;
						AsteroidF.ConstHP = AsteroidF.HP = (rand() %10) +3;
						AsteroidF.PosY = (rand() % 50)*(-1);
						AsteroidF.PosX = rand() % (ResX - 10) + 10;
					}
					al_draw_bitmap_region(Player.Bitmap, Player.CurFrame*Player.FrameW, 0, Player.FrameW, Player.FrameH, Player.PosX - Player.FrameW/2,Player.PosY - Player.FrameH / 2,0);

					if (PlayerCollision == true && Boom.StartAnimation == false)
						Boom.StartAnimation = true;
					if (Boom.StartAnimation == true) {
						al_draw_bitmap_region(Boom.Bitmap, Boom.CurFrame*Boom.FrameW, 0, Boom.FrameW, Boom.FrameH, Player.PosX - Player.FrameW / 2, Player.PosY - Player.FrameH / 2-15, 0);
						ExplosionAnimation(Boom.FrameCount, Boom.FrameDelay, Boom.CurFrame, Boom.MaxFrame, Boom.StartAnimation, EndGame);
					}
					for (int i = MinShoot;i < ShootCounter;i++) {
						al_draw_bitmap(Bullet, ShootsX[i]-al_get_bitmap_width(Bullet)/2, ShootsY[i]-Player.Height/2, 0);
					}
					if(FuelUp.HP > 0)
						al_draw_bitmap(FuelUp.Bitmap, FuelUp.PosX - FuelUp.Width / 2, FuelUp.PosY - FuelUp.Height / 2, 0);
					if (AmmoUp.HP > 0)
						al_draw_bitmap(AmmoUp.Bitmap, AmmoUp.PosX - AmmoUp.Width / 2, AmmoUp.PosY - AmmoUp.Height / 2, 0);
					if (SkullUp.HP > 0)
						al_draw_bitmap(SkullUp.Bitmap, SkullUp.PosX - SkullUp.Width / 2, SkullUp.PosY - SkullUp.Height / 2, 0);

					DrawHUD(Player.Fuel, Player.MaxFuel, Player.Ammo,Player.MaxAmmo, Font);
					al_draw_textf(Font, al_map_rgb(255, 255, 255), ResX / 2, 40, ALLEGRO_ALIGN_CENTRE, "Points: %i", Player.Score);
					al_flip_display();
				}
			}
			if (Player.Score > BestScore)
				SaveHighScore(Player.Score, BestScore);
			FirstRun = false;
		}
		else if (MenuChoice == 1) {
			if (NoSound == false) {
				if (PlayMusic == true) {
					PlayMusic = false;
					al_stop_samples();
				}
				else {
					PlayMusic = true;
					al_play_sample(Music, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
				}
			}
		}
		else if (MenuChoice == 2) {
			End = true;
		}
	}
 }