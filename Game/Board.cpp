/*
 * Board.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Board.h"

#include <iostream>

#include "BoardEventHandler.h"
#include "VsGame.h"

Board::Board(Game* game) :
_game(game),
_state(COUNTDOWN),
_ticksRun(0),
_garbageSpawnPositions{0},
_warnColumns{0},
_cursorX(2),
_cursorY(5),
_tickMatched(0),
_stackOffset(0),
_stackRaiseTicks(10),
_stackRaiseTimer(0),
_stackRaiseForced(false),
_graceTimer(0),
_chainCounter(1),
_tickChain(false),
_tickChainEnd(false),
_lastChain(0),
_blockOnTopRow(false),
_panic(false),
_score(0) {
    fillRandom();
    fillBufferRow();
}

void Board::setEventHandler(BoardEventHandler* eh) {
    _eventHandler = eh;
}

BoardEventHandler* Board::getEventHandler() const {
    return _eventHandler;
}

Board Board::reset() {
    Board reset = Board(_game);
    reset._eventHandler = _eventHandler;
    return reset;
}

Board::Tile::Tile() :
type(AIR),
g(nullptr) {
}

void Board::fillRandom() {

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            _tiles[i][j].type = BLOCK;
            _tiles[i][j].b = Block();
            std::list<int> colors;
            for (int k = 0; k < BlockColor::COUNT; k++) {
                colors.push_back(k);
            }
            if (j - 1 >= 0) {
                colors.remove(_tiles[i][j - 1].b._color);
            }
            if (i - 1 >= 0) {
                colors.remove(_tiles[i - 1][j].b._color);
            }
            _tiles[i][j].b._color = Block::getRandomColor(colors);
        }
    }
}

void Board::fillBufferRow() {
    for (int i = 0; i < BOARD_WIDTH; i++) {
        std::list<int> colors;
        for (int k = 0; k < BlockColor::COUNT; k++) {
            colors.push_back(k);
        }
        colors.remove(_tiles[0][i].b._color);
        colors.remove(_bufferRow[i - 1].b._color);
        _bufferRow[i].type = BLOCK;
        _bufferRow[i].b._color = Block::getRandomColor(colors);
    }
}

void Board::queueGarbage(bool fullWidth, int size, GarbageBlockType type) {
    //TODO: maybe locking for ongoing chains
    GarbageSpawn g = {fullWidth, type, size, 80};
    _garbageQueue.push_back(g); //is there another way to do this properly?
}

void Board::handleGarbageQueue() {
    int y = TOP_ROW;

    //find lowest insertion point
    for (int row = BOARD_HEIGHT - 1; row >= y; --row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            if (_tiles[row][col].type != AIR) {
                y = row + 1;
                goto end;
            }
        }
    }
end:

    for (auto it = _garbageQueue.begin(); it != _garbageQueue.end(); ++it) {

        if (y >= BOARD_HEIGHT) {
            break;
        }
        //block is going to spawn now
        if (--it->spawnTimer <= 0 && !_activeBlocks) {
            //determine spawn position
            int x = 0;
            int w, h;

            if (it->fullWidth) {
                w = BOARD_WIDTH;
                h = it->size;
                y += h - 1;
            } else {
                if (it->size == 3) {
                    x = _garbageSpawnPositions[0]++ % 2 * 3;
                } else if (it->size < 6) {
                    x = _garbageSpawnPositions[it->size - 3]++
                            % (6 - it->size + 1);
                }
                w = it->size;
                h = 1;
            }
            if (spawnGarbage(x, y, w, h, it->type)) {
                ++y;
                it = _garbageQueue.erase(it);
                continue;
            }
        }

    }
}

bool Board::spawnGarbage(int x, int y, int w, int h, GarbageBlockType type) {
    if (y >= BOARD_HEIGHT || x >= BOARD_WIDTH) {
        return false;
    }

    //check that tiles are free
    int r = y - h;
    for (int row = r; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            if (_tiles[row][col].type != AIR) {
                return false;
            }
        }
    }

    _garbageBlocks.push_back(GarbageBlock(x, y, w, h, type));

    for (int row = y; row >= y - (h - 1); --row) {
        for (int col = x; col <= x + (w - 1); ++col) {
            _tiles[row][col].type = GARBAGE;
            _tiles[row][col].g = &_garbageBlocks.back();
        }
    }
    return true;
}

void Board::inputMoveCursor(Direction d) {
    if (_state != RUNNING && _state != COUNTDOWN) {
        return;
    }

    switch (d) {
        case UP:
            if (_cursorY + 1 < 12) {
                ++_cursorY;
                _eventHandler->cursorMove();
            }
            break;
        case DOWN:
            if (_cursorY - 1 >= 0) {
                --_cursorY;
                _eventHandler->cursorMove();
            }
            break;
        case RIGHT:
            if (_cursorX + 1 < BOARD_WIDTH - 1) {
                ++_cursorX;
                _eventHandler->cursorMove();
            }
            break;
        case LEFT:
            if (_cursorX - 1 >= 0) {
                --_cursorX;
                _eventHandler->cursorMove();
            }
            break;
        default:
            break;
    }
}

bool Board::swappable(int row, int col) {
    //prevent block from swapping into empty space with a block above it
    Tile& t = _tiles[row][col];
    return (((t.type == AIR && _tiles[row + 1][col].type != BLOCK)
            || t.type == BLOCK) && t.b._state == NORMAL);
}

bool Board::blockCanFall(int row, int col) {
    if (_tiles[row][col].type != BLOCK || row == 0
            || _tiles[row][col].b._state != NORMAL) {
        return false;
    } else {
        return (_tiles[row - 1][col].type == AIR
                && _tiles[row - 1][col].b._state == NORMAL);
    }
}

bool Board::garbageBlockCanFall(GarbageBlock& gb) {
    bool canFall = true;
    int row = gb.getY() - gb.getH();
    int col = gb.getX();
    int endCol = col + (gb.getW() - 1);
    if (gb.getState() != GarbageBlockState::NORMAL) {
        canFall = false;
    } else {
        for (int i = col; i <= endCol; ++i) {
            if (_tiles[row][i].type != AIR
                    || (_tiles[row][i].type == AIR
                    && _tiles[row][i].b._state != NORMAL)) {
                canFall = false;
                break;
            }
        }
    }
    return canFall;
}

bool Board::matchTiles(int r1, int c1, int r2, int c2) {
    Tile& t1 = _tiles[r1][c1];
    Tile& t2 = _tiles[r2][c2];
    return (t1.type == BLOCK && t2.type == BLOCK
            && (t1.b._state == NORMAL || t1.b._state == MATCHED)
            && (t2.b._state == NORMAL || t2.b._state == MATCHED)
            && !t1.b._falling && !t2.b._falling && t1.b._color == t2.b._color);
}

void Board::inputSwapBlocks() {
    if (_state != RUNNING) {
        return;
    }
    if (!(swappable(_cursorY, _cursorX) && swappable(_cursorY, _cursorX + 1))
            || !(_tiles[_cursorY][_cursorX].type == BLOCK
            || _tiles[_cursorY][_cursorX + 1].type == BLOCK)) {
        return;
    }
    _tiles[_cursorY][_cursorX].b._state = SWAPPING_RIGHT;
    _tiles[_cursorY][_cursorX + 1].b._state = SWAPPING_LEFT;

    _eventHandler->swap();
}

void Board::initTick() {
    _tickMatched = 0;
    _tickChain = false;
    _tickChainEnd = false;
    _activeBlocks = activeBlocks();
    //detect chain end
    if (!_activeBlocks) {
        if (_chainCounter > 1) {
            _tickChainEnd = true;
            _lastChain = _chainCounter;
            _eventHandler->chainEnd(_chainCounter);
        }
        _chainCounter = 1;
    }
    //check top row for blocks
    _blockOnTopRow = blockOnRow(TOP_ROW);
    //check panic
    for (int row = TOP_ROW; row >= PANIC_HEIGHT; --row) {
        if (blockOnRow(row)) {
            _panic = true;
        } else {
            _panic = false;
        }
    }
    //set warn columns
    for (int row = TOP_ROW; row >= WARN_HEIGHT; --row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            if (_tiles[row][col].type != AIR) {
                _warnColumns[col] = true;
            } else {
                _warnColumns[col] = false;
            }
        }
    }

    //speed up stack raising (within 2 mins)
    if (_ticksRun % 1440 == 1439 && _stackRaiseTicks > 0) {
        --_stackRaiseTicks;
    }
    ++_ticksRun;
}

void Board::matchBlocks() {

    auto setMatchedHor = [this](int startIndex, int row, int matched) {
        for (int i = startIndex; i < startIndex + matched; i++) {
            this->_tiles[row][i].b._state = MATCHED;
            ++_tickMatched;
        }
    };

    auto setMatchedVer = [this](int startIndex, int col, int matched) {
        for (int i = startIndex; i < startIndex + matched; i++) {
            this->_tiles[i][col].b._state = MATCHED;
            ++_tickMatched;
        }
    };

    for (int row = 0; row < BOARD_HEIGHT; row++) {
        int matched = 1;
        int matchStartIndex = 0;
        for (int col = 1; col < BOARD_WIDTH; col++) {
            if (matchTiles(row, col, row, col - 1)) {
                matched++;
            } else {
                if (matched >= 3) {
                    setMatchedHor(matchStartIndex, row, matched);
                }
                matchStartIndex = col;
                matched = 1;
            }
        }
        if (matched >= 3) {
            setMatchedHor(matchStartIndex, row, matched);
        }
    }

    for (int col = 0; col < BOARD_WIDTH; col++) {
        int matched = 1;
        int matchStartIndex = 0;
        for (int row = 1; row < BOARD_HEIGHT; row++) {
            if (matchTiles(row, col, row - 1, col)) {
                matched++;
            } else {
                if (matched >= 3) {
                    setMatchedVer(matchStartIndex, col, matched);
                }
                matchStartIndex = row;
                matched = 1;
            }
        }
        if (matched >= 3) {
            setMatchedVer(matchStartIndex, col, matched);
        }
    }
}

//scan the board from top left and set the explosion time for each block, so we get a pretty animation

void Board::handleMatchedBlocks() {
    int matches = 1;
    bool chain = false;
    bool match = false;
    for (int row = BOARD_HEIGHT - 1; row >= 0; --row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            Tile& tile = _tiles[row][col];

            if (tile.type == BLOCK && tile.b._state == MATCHED) {
                tile.b._explOrder = matches - 1;
                if (tile.b._chain && !chain) {
                    chain = true;
                }
                if (!match) {
                    match = true;
                    _tickMatchCol = col;
                    _tickMatchRow = row;
                }
                tile.b._state = EXPLODING;
                tile.b._explosionTimer = 0;
                tile.b._explosionAnimTicks = BASE_EXPLOSION_TICKS
                        + matches * ADD_EXPL_TICKS;
                tile.b._explosionTicks = BASE_EXPLOSION_TICKS
                        + _tickMatched * ADD_EXPL_TICKS;
                triggerNeighbors(row, col);
                ++matches;
            }
            if (tile.b._state == NORMAL && !tile.b._falling && tile.b._chain) {
                tile.b._chain = false;
            }
        }
    }
    if (chain) {
        ++_chainCounter;
        _tickChain = true;
        chainScoring();
    }
}

void Board::handleTriggeredBlocks() {
    //transformation starts after matched blocks clear
    int animStart = BASE_EXPLOSION_TICKS + (_tickMatched + 1) * ADD_EXPL_TICKS;
    int order = _tickMatched + 1;
    //lowest blocks starts transforming first
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = BOARD_WIDTH - 1; col >= 0; --col) {
            Tile& t = _tiles[row][col];
            if (t.type == GARBAGE && t.g->_state == GarbageBlockState::TRIGGERED
                    && t.g->_transformationTimer != 1) {
                t.g->_transformationTimer = 1; //this is ugly! :P but we need to mark which blocks to transform
                t.g->_animationStart = animStart;
                t.g->_explOrder = order;

                animStart += t.g->_w * t.g->_h * ADD_EXPL_TICKS;
                order += t.g->_w * t.g->_h;
            }
        }
    }
    for (auto it = _garbageBlocks.begin(); it != _garbageBlocks.end(); ++it) {
        if (it->getState() == GarbageBlockState::TRIGGERED
                && it->getTransformationTimer() == 1) {
            it->_transformationTimer = 0;
            it->_state = GarbageBlockState::TRANSFORMING;
            it->_transformationTicks = animStart;
        }
    }
}

void Board::clearTile(Tile& tile) {
    tile = Tile();
}

void Board::handleBlockTimers() {
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            Tile& tile = _tiles[row][col];
            if (tile.type == BLOCK && tile.b._state == EXPLODING) {
                ++tile.b._explosionTimer;

                if (tile.b._explosionTimer == tile.b._explosionAnimTicks) {
                    _eventHandler->blockExplode(col, row, _stackOffset,
                            tile.b._explOrder, _chainCounter);
                    _score += 10;
                }

                if (tile.b._explosionTicks == tile.b._explosionTimer) {
                    clearTile(tile);
                    // we need to set the chain flag for the blocks above, and set it on the others above when it's about to fall
                    if (_tiles[row + 1][col].type == BLOCK
                            && _tiles[row + 1][col].b._state == NORMAL) {
                        _tiles[row + 1][col].b._chain = true;
                    }
                }
            }
            if (tile.b._state == SWAPPING_LEFT
                    || tile.b._state == SWAPPING_RIGHT) {
                tile.b._swapTimer++;
                if (tile.b._swapTimer >= SWAP_DELAY) {
                    if (tile.b._state == SWAPPING_RIGHT) {
                        swapBlocks(row, col);
                    }
                }
            }
        }
    }

    for (auto it = _garbageBlocks.begin(); it != _garbageBlocks.end(); ++it) {
        if (it->getState() == GarbageBlockState::TRANSFORMING) {

            ++it->_transformationTimer;

            int animTime = it->_transformationTimer - it->_animationStart;
            if (animTime >= 0 && animTime % ADD_EXPL_TICKS == 0) {
                //int totalTime = it->_w * it->_h * ADD_EXPL_TICKS;
                int block = animTime / ADD_EXPL_TICKS;
                if (block < it->_w * it->_h) {

                    int rx = block % it->_w;
                    int ry = block / it->_w;
                    int x = it->_x + (it->_w - 1) - rx; //block position in grid
                    int y = it->_y - (it->_h - 1) + ry;

                    _eventHandler->blockExplode(x, y, _stackOffset,
                            it->_explOrder + block, _chainCounter);
                }
            }

            if (it->_transformationTimer == it->_transformationTicks) {
                //transform & shrink the block
                it->_transformationTicks = 0;
                it->_transformationTimer = 0;
                it->_state = GarbageBlockState::NORMAL;
                for (int i = 0; i < it->getW(); ++i) {
                    int row = it->getY() - (it->getH() - 1);
                    int col = it->getX() + i;

                    _tiles[row][col].type = BLOCK;
                    _tiles[row][col].g = nullptr;
                    _tiles[row][col].b = it->getBufferRow(i);
                }
                if (--it->_h <= 0) {
                    it = _garbageBlocks.erase(it);
                    continue;
                }
                it->fillBufferRow();
            }
        }
    }

}

void Board::swapBlocks(int row, int col) {
    Tile& swapRight = _tiles[row][col];
    Tile& swapLeft = _tiles[row][col + 1];
    if (swapLeft.b._state == SWAPPING_LEFT) {
        Tile tmp = swapLeft;
        swapLeft = swapRight;
        swapRight = tmp;
    } else {
        std::cout << "CAN'T SWAP!!\n";
    }
    swapRight.b._swapTimer = 0;
    swapRight.b._state = NORMAL;
    swapLeft.b._swapTimer = 0;
    swapLeft.b._state = NORMAL;
}

//sets the chain flag for the block and the ones above it

void Board::setChainAbove(int row, int col) {
    for (int y = row; y < BOARD_HEIGHT; ++y) {
        Tile& tile = _tiles[y][col];
        if (tile.type == BLOCK) {
            tile.b._chain = true;
        }
    }
}

void Board::handleFalling() {
    for (int col = 0; col < BOARD_WIDTH; col++) {
        for (int row = 0; row < BOARD_HEIGHT; row++) {
            Tile& tile = _tiles[row][col];
            if (tile.b._state == FLOATING) { //floating->falling
                if (--tile.b._floatTimer <= 0) {
                    tile.b._state = NORMAL;
                    tile.b._falling = true;
                    for (int y = row; y < BOARD_HEIGHT; ++y) {
                        if (_tiles[y][col].type == BLOCK) {
                            _tiles[y][col].b._falling = true;
                        }
                    }
                    if (tile.b._chain) {
                        setChainAbove(row, col);
                    }
                }
            }
            if (blockCanFall(row, col)) {
                if (!tile.b._falling) { //normal -> floating
                    tile.b._state = FLOATING;
                    tile.b._floatTimer = FLOAT_TICKS;
                } else { //actual falling
                    _tiles[row - 1][col] = _tiles[row][col];
                    clearTile(_tiles[row][col]);
                }
            } else if (tile.b._falling) {
                tile.b._falling = false;
                _eventHandler->blockFall();
            }
        }
    }
}

void Board::handleGarbageFalling() {
    for (auto it = _garbageBlocks.begin(); it != _garbageBlocks.end(); ++it) {
        if (garbageBlockCanFall(*it)) {

            it->_falling = true;

            int row = it->getY() - (it->getH());
            int col = it->getX();
            int endCol = col + (it->getW() - 1);
            //clear top row of gb
            for (int i = col; i <= endCol; ++i) {
                int y = it->getY();
                clearTile(_tiles[y][i]);

                if (y < BOARD_HEIGHT - 1 && _tiles[y + 1][i].type == BLOCK) {
                    _tiles[y + 1][i].b._falling = true;
                }

            }
            //set row below to point to gb
            for (int i = col; i <= endCol; ++i) {
                _tiles[row][i].type = GARBAGE;
                _tiles[row][i].g = &(*it);
            }
            //decrease gb y position
            --it->_y;
        } else if (it->_falling) {
            //handle event when gb stops falling
            bool big = it->_h > 1;
            if (it->_initialFall) {
                _eventHandler->gbFall(big);
            }
            it->_initialFall = false;
            it->_falling = false;
        }
    }
}

void Board::triggerNeighbors(int row, int col) {
    int list[][2] = {
        { 1, 0},
        { 0, 1},
        { -1, 0},
        { 0, -1}
    };
    for (int n = 0; n < 4; ++n) {
        int y = list[n][0];
        int x = list[n][1];
        triggerTile(row + y, col + x, _tiles[row][col]);
    }
}

void Board::triggerGarbageNeighbors(GarbageBlock& g, Tile& t) {
    for (int col = g.getX(); col <= g.getX() + (g.getW() - 1); ++col) {
        triggerTile(g.getY() + 1, col, t);
        triggerTile(g.getY() - g.getH(), col, t);
    }
    for (int row = g.getY(); row >= g.getY() - (g.getH() - 1); --row) {
        triggerTile(row, g.getX() - 1, t);
        triggerTile(row, g.getX() + g.getW(), t);
    }
}

void Board::triggerTile(int row, int col, Tile& triggerer) {
    if (row < 0 || col < 0 || row >= BOARD_HEIGHT || col >= BOARD_WIDTH) {
        return;
    }
    Tile& triggered = _tiles[row][col];
    if (triggered.type == GARBAGE) {
        if (triggered.g->getState() == GarbageBlockState::NORMAL) {
            if (triggerer.type != GARBAGE
                    || (triggerer.type == GARBAGE
                    && triggerer.g->getType() == triggered.g->getType())) {
                triggered.g->trigger();
                triggerGarbageNeighbors(*triggered.g, triggerer);
            }
        }
    }
}

void Board::raiseStack() {
    //increase the timer
    if (!_stackRaiseForced && _stackRaiseTimer < _stackRaiseTicks) {
        ++_stackRaiseTimer;
        return;
    }
    //raise the stack one step
    if (_stackOffset < STACK_RAISE_STEPS && !_activeBlocks && !_blockOnTopRow) {
        ++_stackOffset;
        _stackRaiseTimer = 0;
    }
    //when blocks are on top row
    if (_blockOnTopRow && !_activeBlocks) {
        if (++_graceTimer >= (STACK_RAISE_STEPS * _stackRaiseTicks) / 2) {
            _state = GAME_OVER;
        }
    }
    if (!_blockOnTopRow) {
        _graceTimer = 0;
    }

    //shift up the blocks and buffer row
    if (_stackOffset >= STACK_RAISE_STEPS) {
        if (!_blockOnTopRow && !_activeBlocks) {
            for (auto it = _garbageBlocks.begin(); it != _garbageBlocks.end();
                    ++it) {
                ++it->_y;
            }
            for (int row = BOARD_HEIGHT - 2; row >= 0; --row) {
                for (int col = 0; col < BOARD_WIDTH; ++col) {
                    _tiles[row + 1][col] = _tiles[row][col];
                }
            }
            for (int i = 0; i < BOARD_WIDTH; ++i) {
                _tiles[0][i] = _bufferRow[i];
            }
            fillBufferRow();
            _stackOffset = 0;

            if (_stackRaiseForced) {
                ++_score;
            }

            _stackRaiseForced = false;
            if (_cursorY <= 10) {
                ++_cursorY;
            }
        }
    }
}

void Board::inputForceStackRaise() {
    if (_state != RUNNING) {
        return;
    }
    if (!_activeBlocks) {
        _stackRaiseForced = true;
    }
}

bool Board::activeBlocks() {
    for (Tile* it = &_tiles[0][0];
            it != &_tiles[0][0] + BOARD_WIDTH * BOARD_HEIGHT; ++it) {
        if (it->type == BLOCK && (it->b._state != NORMAL || it->b._falling)) {
            return true;
        }
    }
    for (auto it = _garbageBlocks.begin(); it != _garbageBlocks.end(); ++it) {
        if (it->getState() != GarbageBlockState::NORMAL || it->_falling) {
            return true;
        }
    }
    return false;
}

void Board::tick() {
    if (_state == COUNTDOWN) {
        if (_game->getTime() > COUNTDOWN_MS) {
            _state = RUNNING;
        }
    } else if (_state == RUNNING) {
        initTick();
        raiseStack();
        handleGarbageQueue();
        handleBlockTimers();
        handleFalling();
        handleGarbageFalling();
        matchBlocks();
        handleMatchedBlocks();
        handleTriggeredBlocks();

        sendEvents();
    }
    _eventHandler->endTick();
}

void Board::win() {
    _state = WON;
}

Board::~Board() {
}

bool Board::hasActiveBlocks() const {
    return _activeBlocks;
}

const Board::Tile& Board::getBufferRow(int col) const {
    return _bufferRow[col];
}

int Board::getChainCounter() const {
    return _chainCounter;
}

int Board::getCursorX() const {
    return _cursorX;
}

int Board::getCursorY() const {
    return _cursorY;
}

int Board::getStackOffset() const {
    return _stackOffset;
}

bool Board::isStackRaiseForced() const {
    return _stackRaiseForced;
}

int Board::getStackRaiseTicks() const {
    return _stackRaiseTicks;
}

int Board::getStackRaiseTimer() const {
    return _stackRaiseTimer;
}

Board::BoardState Board::getState() const {
    return _state;
}

bool Board::isTickChain() const {
    return _tickChain;
}

int Board::getTickMatchCol() const {
    return _tickMatchCol;
}

int Board::getTickMatched() const {
    return _tickMatched;
}

int Board::getTickMatchRow() const {
    return _tickMatchRow;
}

int Board::getGraceTimer() const {
    return _graceTimer;
}

const Board::Tile& Board::getTile(int row, int col) const {
    return _tiles[row][col];
}

const std::list<GarbageBlock>& Board::getGarbageBlocks() const {
    return _garbageBlocks;
}

const std::list<Board::GarbageSpawn>& Board::getGarbageQueue() const {
    return _garbageQueue;
}

bool Board::isTickChainEnd() const {
    return _tickChainEnd;
}

int Board::getLastChain() const {
    return _lastChain;
}

unsigned int Board::getTicksRun() const {
    return _ticksRun;
}

Game& Board::getGame() const {
    return *_game;
}

uint32_t Board::getTime() const {
    return _game->getTime();
}

bool Board::blockOnRow(int row) {
    for (int col = 0; col < BOARD_WIDTH; ++col) {
        Tile& t = _tiles[row][col];
        if (t.type != AIR && !(t.type == GARBAGE && t.g->_initialFall)) {
            return true;
        }
    }
    return false;
}

bool Board::getWarnColumn(int col) const {
    return _warnColumns[col];
}

int Board::getScore() const {
    return _score;
}

void Board::sendEvents() {
    if (_tickChain) {
        _eventHandler->chain(_chainCounter, _tickMatchCol, _tickMatchRow,
                _stackOffset);
    }
    if (_tickMatched > 3) {
        _eventHandler->combo(_tickMatched, _tickMatchCol, _tickMatchRow,
                _stackOffset);
        chainScoring();
        comboScoring();
    }
}

bool Board::isPanic() const {
    return _panic;
}

void Board::chainScoring() {
    switch (_chainCounter) {
        case 1:
            break;
        case 2:
            _score += 50;
            break;
        case 3:
            _score += 80;
            break;
        case 4:
            _score += 150;
            break;
        case 5:
            _score += 300;
            break;
        default:
            _score += (_chainCounter + 2) * 200;
            break;
    }
}

void Board::comboScoring() {
    if (_tickMatched == 4) {
        _score += 20;
    } else if (_tickMatched == 5) {
        _score += 30;
    } else if (_tickMatched >= 6) {
        _score += (_chainCounter - 4) * 20;
    }
}


