#pragma once

#include "oxygine-framework.h"
// #include <functional>

// using namespace oxygine;

#include <array>
#include <vector>
#include <string>
// #include <iostream>

extern oxygine::Resources gameResources;

class AbstractChessFigure;
DECLARE_SMART(AbstractChessFigure, spAbstractChessFigure);

enum Chess {pawn , horse, rock, bishop, queen, king};
enum Field_type {NoneChess, BlackChess, WhiteChess};

const int border_size = 40;
const int cell_size = 82;

const int size_chess_board = 8;

Field_type invertColorField(Field_type a);

oxygine::Vector2 formPosToNewPos( const oxygine::Vector2 & position , oxygine::Vector2 size );

oxygine::Point formPosToCellPos( const oxygine::Vector2 & position , oxygine::Vector2 size);

oxygine::Vector2 cellToPos(oxygine::Point cellNum);

class Field
{

public:

	//Матрица указателей на фигуры (Полиморфизм)
	std::array<
		std::array< spAbstractChessFigure, size_chess_board >,
		size_chess_board
	> pArray;

	//Этот метод должен быть определён после класса AbstractChessFigure
	void deleteChese(oxygine::Point cellPos);

	//Этот метод должен быть определён после класса AbstractChessFigure
	Field_type getFieldType(oxygine::Point pos);

	Field();

	bool isFreePlace(oxygine::Point pnt);
	
	void setPlace(oxygine::Point PrevCellPos, oxygine::Point cellPos); //Переставляем фигуру

};

extern Field mainField;


class AbstractChessFigure: public oxygine::Sprite
{
	//-1 - значит что предыдущей позиции небыло (при инициализации)
	oxygine::Point PrevCellPos = oxygine::Point(-1, -1);

	Field_type fieldT;

	std::vector<oxygine::spSprite> next_steps;

public:

	AbstractChessFigure(int x_cell , int y_cell , Field_type fieldT_, Chess chess_type);
	
	
	virtual ~AbstractChessFigure();

	Field_type getFieldT();

	virtual std::vector<oxygine::Point> getPossibleSteps(oxygine::Point pos) =0;

	void showPossibleSteps();

	void hidePossibleSteps();

	void setPositionByCell(oxygine::Point cellNum);

	oxygine::Point getPositionByCell();

	void onEvent(oxygine::Event* ev);

	void move(const oxygine::Vector2& pos);

	void doUpdate(const oxygine::UpdateState& us);

	void onAdded2Stage();

	void onRemovedFromStage();

	oxygine::Vector2 local;
};

//========begin[Определения шахматных фигур (как они ходят)]============

class Pawn: public AbstractChessFigure
{

	bool isFirstStep;

public:

	Pawn(int x_cell , int y_cell , Field_type fieldT_ ):
		AbstractChessFigure(x_cell, y_cell , fieldT_ , Chess::pawn)
	{};
	
	virtual std::vector<oxygine::Point> getPossibleSteps(oxygine::Point pos);

};

class Rock: virtual public AbstractChessFigure 
//virtual т.к. The diamond problem в классе Queen (который наследуеться от этого)
{

public:

	Rock(int x_cell , int y_cell , Field_type fieldT_ ):
		AbstractChessFigure(x_cell, y_cell , fieldT_ , Chess::rock)
	{};
	
	virtual std::vector<oxygine::Point> getPossibleSteps(oxygine::Point pos);

};

class Horse: public AbstractChessFigure
{

public:

	Horse(int x_cell , int y_cell , Field_type fieldT_ ):
		AbstractChessFigure(x_cell, y_cell , fieldT_ , Chess::horse)
	{};
	
	virtual std::vector<oxygine::Point> getPossibleSteps(oxygine::Point pos);

};

class Bishop: virtual public AbstractChessFigure
//virtual т.к. The diamond problem в классе Queen (который наследуеться от этого)
{

public:

	Bishop(int x_cell , int y_cell , Field_type fieldT_ ):
		AbstractChessFigure(x_cell, y_cell , fieldT_ , Chess::bishop)
	{};
	
	virtual std::vector<oxygine::Point> getPossibleSteps(oxygine::Point pos);

};

class King: public AbstractChessFigure
{

public:

	King(int x_cell , int y_cell , Field_type fieldT_ ):
		AbstractChessFigure(x_cell, y_cell , fieldT_ , Chess::king)
	{};
	
	virtual std::vector<oxygine::Point> getPossibleSteps(oxygine::Point pos);

};

class Queen: public Bishop, public Rock
{

public:

	Queen(int x_cell , int y_cell , Field_type fieldT_ ):
		AbstractChessFigure(x_cell, y_cell , fieldT_ , Chess::queen),
		Bishop(x_cell, y_cell , fieldT_),
		Rock(x_cell, y_cell , fieldT_)
	{};
	
	virtual std::vector<oxygine::Point> getPossibleSteps(oxygine::Point pos);

};

//========end[Определения шахматных фигур (как они ходят)]============
