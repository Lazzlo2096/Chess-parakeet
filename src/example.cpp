#include "oxygine-framework.h"
// #include <functional>

using namespace oxygine;

#include <array>
#include <vector>
#include <string>
// #include <iostream>

#include "ChessObjects.h"

Resources gameResources;
Field mainField;


class MainActor: public Actor
{

public:

	spSprite bg; // Фон

	MainActor(){

		bg = new Sprite;
		bg->setResAnim(gameResources.getResAnim("board"));
		addChild(bg);

		// Chess localChess;
		Field_type localFieldT;
		int row;
		int row2;

		// расставление фигур по доске (mainField) и их прикрепление к MainActor
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 2; ++j)
			{
				if(j==1){
					localFieldT = Field_type::BlackChess;
					row = 0;
					row2 = 1;
				}else{
					localFieldT = Field_type::WhiteChess;
					row = 7;
					row2 = 6;
				}

				if (i==0 || i==7)
					mainField.pArray[i][row] = new Rock(i, row ,localFieldT);
				if (i==1 || i==6)
					mainField.pArray[i][row] = new Horse(i, row ,localFieldT);
				if (i==2 || i==5)	
					mainField.pArray[i][row] = new Bishop(i, row ,localFieldT);
				if (i==3)
					mainField.pArray[i][row] = new Queen(i, row ,localFieldT);
				if (i==4)
					mainField.pArray[i][row] = new King(i, row ,localFieldT);
			
				addChild(mainField.pArray[i][row]);
				
				mainField.pArray[i][row2] = new Pawn(i, row2 ,localFieldT);
				addChild(mainField.pArray[i][row2]);
			}
	}

};
DECLARE_SMART(MainActor,spMainActor)


void example_preinit() {}

//called from main.cpp
void example_init()
{
	//load xml file with resources definition
	gameResources.loadXML("res.xml");

	spMainActor actor = new MainActor;
	getStage()->addChild(actor);

}

//called each frame from main.cpp
void example_update()
{

}

//called each frame from main.cpp
void example_destroy()
{
	//free previously loaded resources
	gameResources.free();
}
