#include "ChessObjects.h"


Field_type invertColorField(Field_type a){
	if (a==Field_type::BlackChess)
		return Field_type::WhiteChess;
	else if (a==Field_type::WhiteChess)
		return Field_type::BlackChess;
}

oxygine::Vector2 formPosToNewPos( const oxygine::Vector2 & position , oxygine::Vector2 size ){
//Выравнивание объекта по клеткам шахматной доски
	auto half_sprite_size = size / 2;
	auto centerPos = position + half_sprite_size;
	oxygine::Vector2 newPosition(
		centerPos.x-(int(centerPos.x-border_size)% cell_size),
		centerPos.y-(int(centerPos.y-border_size)% cell_size)
	);
	return newPosition;
}

oxygine::Point formPosToCellPos( const oxygine::Vector2 & position , oxygine::Vector2 size){
//Из векторных координат в координаты шахмотной доски
	auto half_sprite_size = size / 2;
	auto centerPos = position + half_sprite_size;
	oxygine::Point cellPos(
		int(centerPos.x-border_size) / cell_size,
		int(centerPos.y-border_size) / cell_size
	);
	return cellPos;
}

oxygine::Vector2 cellToPos(oxygine::Point cellNum){

	oxygine::Vector2 newPosition(
		border_size+(cellNum.x * cell_size)+1, //+1 для красоты
		border_size+(cellNum.y * cell_size)+1  //+1 для красоты
	);
	return newPosition;
}



//---------------
Field::Field(){
	for (int i = 0; i < size_chess_board; ++i)
		for (int j = 0; j < size_chess_board; ++j)
		{
			pArray[i][j] = nullptr;
		}
};

bool Field::isFreePlace(oxygine::Point pnt){
	return getFieldType(pnt)==Field_type::NoneChess? true: false;
}

void Field::setPlace(oxygine::Point PrevCellPos, oxygine::Point cellPos) //Переставляем фигуру
{
	if (PrevCellPos.x != -1)
		pArray[cellPos.x][cellPos.y].swap( pArray[PrevCellPos.x][PrevCellPos.y] );
}


void Field::deleteChese(oxygine::Point cellPos){
	(pArray[cellPos.x][cellPos.y])->detach();
	pArray[cellPos.x][cellPos.y] = nullptr;
	//stage removeChild()
}

Field_type Field::getFieldType(oxygine::Point pos){
	// return _field[pos.x][pos.y];
	return !pArray[pos.x][pos.y]? Field_type::NoneChess : pArray[pos.x][pos.y]->getFieldT(); 
}
//---------------




AbstractChessFigure::AbstractChessFigure(int x_cell , int y_cell , Field_type fieldT_, Chess chess_type)
{

	std::string typeChees;
	// drag.init(this);
	switch(chess_type){
		case Chess::pawn: typeChees = "pawn";	break;
		case Chess::horse: typeChees = "horse";	break;
		case Chess::rock: typeChees = "rock";	break;
		case Chess::bishop: typeChees = "bishop";	break;
		case Chess::queen: typeChees = "queen";	break;
		case Chess::king: typeChees = "king";	break;
	}

	fieldT = fieldT_;
	if(fieldT == Field_type::BlackChess)
		typeChees+="-black";

	setResAnim(gameResources.getResAnim(typeChees)); 

	oxygine::Point newCellPosition(x_cell, y_cell);
	setPositionByCell( newCellPosition );

	PrevCellPos = newCellPosition;

}

AbstractChessFigure::~AbstractChessFigure(){
	// std::cout << "*Дестркутор шахматы" << std::endl;
}

Field_type AbstractChessFigure::getFieldT(){ return fieldT; }

// virtual std::vector<oxygine::Point> getPossibleSteps(oxygine::Point pos) =0;

void AbstractChessFigure::showPossibleSteps(){

	std::vector<oxygine::Point> possibleSteps = getPossibleSteps(getPositionByCell());

	for( oxygine::Point i : possibleSteps){
		next_steps.push_back( new Sprite() );
		next_steps.back()->setResAnim(gameResources.getResAnim("step"));
		next_steps.back()->setPosition( cellToPos(i) );
		oxygine::getStage()->addChild(next_steps.back());
	}

}

void AbstractChessFigure::hidePossibleSteps(){
	
	size_t sz = next_steps.size();

	for(int i=0; i!=sz; i++){
		oxygine::getStage()->removeChild(next_steps.back());
		next_steps.pop_back();
	}
}

void AbstractChessFigure::setPositionByCell(oxygine::Point cellNum){
	setPosition( cellToPos(cellNum) );
	mainField.setPlace( PrevCellPos, cellNum );
}

oxygine::Point AbstractChessFigure::getPositionByCell(){
	return formPosToCellPos(getPosition(), getSize());
}

void AbstractChessFigure::onEvent(oxygine::Event* ev)
{
	oxygine::TouchEvent* te = oxygine::safeCast<oxygine::TouchEvent*>(ev);
	if (te->type == oxygine::TouchEvent::TOUCH_DOWN){
		local = te->localPosition;
		_stage->addEventListener(oxygine::TouchEvent::MOVE,
			CLOSURE(this, &AbstractChessFigure::onEvent));
		_stage->addEventListener(oxygine::TouchEvent::TOUCH_UP,
			CLOSURE(this, &AbstractChessFigure::onEvent));

		showPossibleSteps();
	}
	if (te->type == oxygine::TouchEvent::MOVE){
		move(te->localPosition);
	}
	if (te->type == oxygine::TouchEvent::TOUCH_UP)
	{
		
		hidePossibleSteps();

		//координаты Левого-верхниего взятого мышкой объекта
		oxygine::Vector2 whereMousUp = getPosition();

		//выровненная позиция whereMousUp на доске по центрам клеточек
		oxygine::Vector2 alignPos = formPosToNewPos(
			whereMousUp,
			getSize()
		);

		auto newCellPos = formPosToCellPos(
			alignPos
			,getSize()
		);

		bool isNewCellFree = mainField.isFreePlace( newCellPos );

		/*whereMousUp.x - border_size = Это whereMousUp,
		относительно клеток доски шахмат*/
		bool isNewPosInFieldByCornerOne = 
			(8*cell_size+getSize().x/2 > whereMousUp.x-border_size) 
			&& (8*cell_size+getSize().y/2 > whereMousUp.y-border_size);
		bool isNewPosInFieldByCornerTwo = 
			(whereMousUp.x-border_size > 0-getSize().x/2 )
			&& (whereMousUp.y-border_size > 0-getSize().y/2 );

		bool newCellPos_is_possibleStep = false;
		std::vector<oxygine::Point> possibleSteps = getPossibleSteps( PrevCellPos );
		for (oxygine::Point possibleStep: possibleSteps)
			newCellPos_is_possibleStep = newCellPos_is_possibleStep ||
				(possibleStep == newCellPos);

		if( newCellPos_is_possibleStep &&
			isNewPosInFieldByCornerOne && isNewPosInFieldByCornerTwo )
		{
			if( isNewCellFree ) 
			{

				// std::cout << "свободно" << std::endl;

				setPositionByCell( newCellPos );

				// PrevCellPos = getPositionByCell();
				PrevCellPos = newCellPos;
				
			}else if( mainField.getFieldType(newCellPos)==invertColorField(fieldT) )
			//если на этом месте (newCellPos) фигура другого цвета
			{

				// std::cout << "занято другим цветом! Скушать!" << std::endl;

				mainField.deleteChese( newCellPos );

				setPositionByCell( newCellPos );
				
				// PrevCellPos = getPositionByCell();
				PrevCellPos = newCellPos;

			}else{
				// std::cout << "занято своим цветом" << std::endl;

				setPositionByCell( PrevCellPos );
			};

		}else{
			// std::cout << "Игрок хочет поставить фигуру за пределы поля" << std::endl;
			// std::cout << "или Эта фигура так не ходит" << std::endl;

			setPositionByCell( PrevCellPos );
		};
			
		_stage->removeEventListeners(this);

	}//if (te->type == oxygine::TouchEvent::TOUCH_UP)
}

void AbstractChessFigure::move(const oxygine::Vector2& pos)
{
	oxygine::Vector2 localPos = stage2local(pos);
	oxygine::Vector2 offset = localPos - local;
	oxygine::Transform tr = getTransform();
	tr.x = 0;
	tr.y = 0;
	oxygine::Vector2 p = tr.transform(offset);
	setPosition(getPosition() + p );
}

void AbstractChessFigure::doUpdate(const oxygine::UpdateState& us)
{
	oxygine::pointer_index ind = getPressed();
	if (!ind)
		return;
	oxygine::PointerState* st = oxygine::Input::instance.getTouchByIndex(ind);
	move(_stage->parent2local(st->getPosition()));
}

void AbstractChessFigure::onAdded2Stage()
{
	addEventListener(oxygine::TouchEvent::TOUCH_DOWN, 
		CLOSURE(this, &AbstractChessFigure::onEvent)
	);
}

void AbstractChessFigure::onRemovedFromStage()
{
	_stage->removeEventListeners(this);
}



//========begin[Определения шахматных фигур (как они ходят)]============


std::vector<oxygine::Point> Pawn::getPossibleSteps(oxygine::Point pos){

	std::vector<oxygine::Point> possibleSteps;

	oxygine::Point newPoint;

	short sign;
	if (getFieldT()==Field_type::WhiteChess)
		sign = -1;
	else
		sign = 1;

	// if(pos.y>0)
	newPoint = oxygine::Point(pos.x, pos.y+1*sign);

	bool isFirstStep = getFieldT()==Field_type::WhiteChess? pos.y==6 : pos.y==1;

	if(isFirstStep){
		possibleSteps.push_back(oxygine::Point(pos.x, pos.y+2*sign));
		isFirstStep = false;
	}
	
	if(newPoint.x >=0 and newPoint.x <=7 and newPoint.y>=0 and newPoint.y<=7)
		possibleSteps.push_back(newPoint);

	return possibleSteps;
};


std::vector<oxygine::Point> Rock::getPossibleSteps(oxygine::Point pos){

	std::vector<oxygine::Point> possibleSteps;

	std::vector<oxygine::Point> newPoints;

	for(int i=0; i!=size_chess_board; i++)
		newPoints.push_back(oxygine::Point(pos.x, i));

	for(int i=0; i!=size_chess_board; i++)
		newPoints.push_back(oxygine::Point(i, pos.y));

	for(auto newPoint: newPoints)
		if(newPoint.x >=0 and newPoint.x <=7 and
			newPoint.y>=0 and newPoint.y<=7 and (newPoint!=pos)
		)
			possibleSteps.push_back(newPoint);

	return possibleSteps;
};



std::vector<oxygine::Point> Horse::getPossibleSteps(oxygine::Point pos){

	std::vector<oxygine::Point> possibleSteps;

	std::vector<oxygine::Point> newPoints;

	newPoints.push_back(oxygine::Point(pos.x+1, pos.y+2));
	newPoints.push_back(oxygine::Point(pos.x-1, pos.y+2));
	newPoints.push_back(oxygine::Point(pos.x+1, pos.y-2));
	newPoints.push_back(oxygine::Point(pos.x-1, pos.y-2));

	newPoints.push_back(oxygine::Point(pos.x+2, pos.y+1));
	newPoints.push_back(oxygine::Point(pos.x-2, pos.y+1));
	newPoints.push_back(oxygine::Point(pos.x+2, pos.y-1));
	newPoints.push_back(oxygine::Point(pos.x-2, pos.y-1));

	for(auto newPoint: newPoints)
		if(newPoint.x >=0 and newPoint.x <=7 and newPoint.y>=0 and newPoint.y<=7)
			possibleSteps.push_back(newPoint);

	return possibleSteps;
};



std::vector<oxygine::Point> Bishop::getPossibleSteps(oxygine::Point pos){

	std::vector<oxygine::Point> possibleSteps;

	std::vector<oxygine::Point> newPoints;

	for(int i=0; i!=size_chess_board*2; i++)
		newPoints.push_back(oxygine::Point(pos.x+i-size_chess_board, pos.y+i-size_chess_board));

	for(int i=0; i!=size_chess_board*2; i++)
		newPoints.push_back(oxygine::Point(pos.x-i+size_chess_board, pos.y+i-size_chess_board));

	for(auto newPoint: newPoints)
		if(newPoint.x >=0 and newPoint.x <=7 and
			newPoint.y>=0 and newPoint.y<=7 and (newPoint!=pos)
		)
			possibleSteps.push_back(newPoint);

	return possibleSteps;
};


std::vector<oxygine::Point> King::getPossibleSteps(oxygine::Point pos){

	std::vector<oxygine::Point> possibleSteps;

	std::vector<oxygine::Point> newPoints;

	for(int i=-1; i!=2; i++)
	for(int j=-1; j!=2; j++)
		newPoints.push_back(oxygine::Point(pos.x+i, pos.y+j));

	for(auto newPoint: newPoints)
		if(newPoint.x >=0 and newPoint.x <=7 and
			newPoint.y>=0 and newPoint.y<=7 and (newPoint!=pos)
		)
			possibleSteps.push_back(newPoint);

	return possibleSteps;
};

std::vector<oxygine::Point> Queen::getPossibleSteps(oxygine::Point pos){

	std::vector<oxygine::Point> rockPossibleSteps = Rock::getPossibleSteps(pos);
	std::vector<oxygine::Point> possibleSteps = Bishop::getPossibleSteps(pos);

	possibleSteps.insert(possibleSteps.end(),
		rockPossibleSteps.begin(), rockPossibleSteps.end()
	);

	return possibleSteps;
};


//========end[Определения шахматных фигур (как они ходят)]============
