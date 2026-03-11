#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <cmath>
#include <limits>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>

using namespace std;

// 棋盘大小
const int BOARD_SIZE = 8;

// 游戏状态
enum GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

// 难度级别
enum Difficulty {
    EASY = 0,
    MEDIUM = 1,
    HARD = 2
};

// 数字所代表的棋子
enum Piece {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2,
    ARROW = 3
};

// 走法结构
struct Move {
    int from_x, from_y;     // 起点
    int to_x, to_y;         // 终点
    int arrow_x, arrow_y;   // 箭的位置
    
    Move(int fx, int fy, int tx, int ty, int ax, int ay)
        : from_x(fx), from_y(fy), to_x(tx), to_y(ty), arrow_x(ax), arrow_y(ay) {}
    
    Move() : from_x(-1), from_y(-1), to_x(-1), to_y(-1), arrow_x(-1), arrow_y(-1) {}
    
    void print() const {
        cout << "(" << from_x << "," << from_y << ") -> (" << to_x << "," << to_y << ") arrow: (" << arrow_x << "," << arrow_y << ")" << endl;
    }
    
    bool isValid() const {
        return from_x != -1;
    }
};

// 方向数组
const int DIRECTIONS[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1},
    {0, -1},           {0, 1},
    {1, -1},  {1, 0},  {1, 1}
};

class AmazonsGame {
private:
    int board[BOARD_SIZE][BOARD_SIZE];
    int currentPlayer;  // 1: 黑棋, 2: 白棋
    vector<pair<int, int>> blackPieces;
    vector<pair<int, int>> whitePieces;
    Difficulty difficulty;
    
public:
    AmazonsGame() {
        initializeBoard();
    }
    
    // 设置难度
    void setDifficulty(Difficulty diff) {
        difficulty = diff;
    }
    
    // 获取难度
    Difficulty getDifficulty() const {
        return difficulty;
    }
    
    // 初始化棋盘
    void initializeBoard() {
        // 清空棋盘
        memset(board, EMPTY, sizeof(board));
        
        // 初始化黑方棋子
        blackPieces.clear();
        blackPieces.push_back({0, 2});
        blackPieces.push_back({2, 0});
        blackPieces.push_back({5, 0});
        blackPieces.push_back({7, 2});
        
        for (auto& pos : blackPieces) {
            board[pos.first][pos.second] = BLACK;
        }
        
        // 初始化白方棋子
        whitePieces.clear();
        whitePieces.push_back({0, 5});
        whitePieces.push_back({2, 7});
        whitePieces.push_back({5, 7});
        whitePieces.push_back({7, 5});
        
        for (auto& pos : whitePieces) {
            board[pos.first][pos.second] = WHITE;
        }
        
        currentPlayer = BLACK;  // 黑方先手
    }
    
    // 检查位置是否在棋盘内
    bool isValidPosition(int x, int y) const {
        return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
    }
    
    // 获取每一个棋子所有可达位置
    vector<pair<int, int>> getReachablePositions(int start_x, int start_y) const {
        vector<pair<int, int>> positions;
        
        for (int i = 0; i < 8; i++) {
            int dx = DIRECTIONS[i][0];
            int dy = DIRECTIONS[i][1];
            int x = start_x + dx;
            int y = start_y + dy;
            
            while (isValidPosition(x, y) && board[x][y] == EMPTY) {
                positions.push_back({x, y});
                x += dx;
                y += dy;
            }
        }
        
        return positions;
    }
    
    // 生成所有合法走法
    vector<Move> generateAllMoves(int player) const {
        vector<Move> moves;
        const vector<pair<int, int>>& pieces = (player == BLACK) ? blackPieces : whitePieces;
        
        for (const auto& piece : pieces) {
            int from_x = piece.first;
            int from_y = piece.second;
            
            // 获取棋子所有可移动到的位置
            vector<pair<int, int>> movePositions = getReachablePositions(from_x, from_y);
            
            for (const auto& movePos : movePositions) {
                int to_x = movePos.first;
                int to_y = movePos.second;
                
                // 临时移动棋子
                int temp_board[BOARD_SIZE][BOARD_SIZE];
                memcpy(temp_board, board, sizeof(board));
                temp_board[from_x][from_y] = EMPTY;
                temp_board[to_x][to_y] = player;
                
                // 获取所有可放箭的位置
                vector<pair<int, int>> arrowPositions;
                for (int i = 0; i < 8; i++) {
                    int dx = DIRECTIONS[i][0];
                    int dy = DIRECTIONS[i][1];
                    int x = to_x + dx;
                    int y = to_y + dy;
                    
                    while (isValidPosition(x, y) && temp_board[x][y] == EMPTY) {
                        arrowPositions.push_back({x, y});
                        x += dx;
                        y += dy;
                    }
                }
                
                for (const auto& arrowPos : arrowPositions) {
                    moves.push_back(Move(from_x, from_y, to_x, to_y, arrowPos.first, arrowPos.second));
                }
            }
        }
        
        return moves;
    }
    
    // 执行走法
    void makeMove(const Move& move) {
        int player = currentPlayer;
        
        // 移动棋子
        board[move.from_x][move.from_y] = EMPTY;
        board[move.to_x][move.to_y] = player;
        
        // 更新棋子位置列表
        vector<pair<int, int>>& pieces = (player == BLACK) ? blackPieces : whitePieces;
        for (auto& pos : pieces) {
            if (pos.first == move.from_x && pos.second == move.from_y) {
                pos.first = move.to_x;
                pos.second = move.to_y;
                break;
            }
        }
        
        // 放置箭
        board[move.arrow_x][move.arrow_y] = ARROW;
        
        // 切换玩家
        currentPlayer = (player == BLACK) ? WHITE : BLACK;
    }
    
    // 撤销走法
    void undoMove(const Move& move) {
        // 切换回原来的玩家
        currentPlayer = (currentPlayer == BLACK) ? WHITE : BLACK;
        int player = currentPlayer;
        
        // 移除箭
        board[move.arrow_x][move.arrow_y] = EMPTY;
        
        // 恢复棋子位置
        board[move.to_x][move.to_y] = EMPTY;
        board[move.from_x][move.from_y] = player;
        
        // 更新棋子位置列表
        vector<pair<int, int>>& pieces = (player == BLACK) ? blackPieces : whitePieces;
        for (auto& pos : pieces) {
            if (pos.first == move.to_x && pos.second == move.to_y) {
                pos.first = move.from_x;
                pos.second = move.from_y;
                break;
            }
        }
    }
    
    // 检查游戏是否结束
    bool isGameOver() const {
        return generateAllMoves(currentPlayer).empty();
    }
    
    // 下面是简单难度的代码：直接随机下棋
    Move getRandomMove() {
        vector<Move> moves = generateAllMoves(currentPlayer);
        
        if (moves.empty()) {
            return Move(-1, -1, -1, -1, -1, -1);
        }
        
        // 随机选择一个移动
        srand(time(0));
        int randomIndex = rand() % moves.size();
        return moves[randomIndex];
    }
    
    // 以下为四个维度的评估函数
    
    // 1.计算移动力（可走的步法数量）
    int calculateMobility(int player) const {
        return generateAllMoves(player).size();
    }
    
    // 2.计算区域控制（使用BFS)
    int calculateTerritory(int player) const {
        vector<vector<bool>> visited(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));
        int territory = 0;
        
        const vector<pair<int, int>>& pieces = (player == BLACK) ? blackPieces : whitePieces;
        
        for (const auto& piece : pieces) {
            // 从每个棋子开始BFS
            vector<pair<int, int>> queue;
            queue.push_back(piece);
            visited[piece.first][piece.second] = true;
            
            while (!queue.empty()) {
                auto current = queue.back();
                queue.pop_back();
                
                int x = current.first;
                int y = current.second;
                
                // 如果是空地，计入领地
                if (board[x][y] == EMPTY) {
                    territory++;
                }
                
                // 向八个方向扩展
                for (int i = 0; i < 8; i++) {
                    int nx = x + DIRECTIONS[i][0];
                    int ny = y + DIRECTIONS[i][1];
                    
                    if (isValidPosition(nx, ny) && !visited[nx][ny]) {
                        // 只有空地才能扩展
                        if (board[nx][ny] == EMPTY) {
                            visited[nx][ny] = true;
                            queue.push_back({nx, ny});
                        }
                    }
                }
            }
        }
        
        return territory;
    }
    
    // 3.评估棋子位置
    int evaluatePiecePosition(int player) const {
        int score = 0;
        const vector<pair<int, int>>& pieces = (player == BLACK) ? blackPieces : whitePieces;
        
        // 中心位置权重更高
        for (const auto& piece : pieces) {
            int x = piece.first;
            int y = piece.second;
            
            // 距离中心越近越好
            int distance = abs(x - 3.5) + abs(y - 3.5);
            score += (7 - distance);  // 最大距离为7
            
            // 边角位置惩罚
            if (x == 0 || x == BOARD_SIZE-1 || y == 0 || y == BOARD_SIZE-1) {
                score -= 2;
            }
        }
        
        return score;
    }
    
    // 4.评估棋子灵活性
    int evaluatePieceFlexibility(int player) const {
        int flexibility = 0;
        const vector<pair<int, int>>& pieces = (player == BLACK) ? blackPieces : whitePieces;
        
        for (const auto& piece : pieces) {
            // 计算该棋子可移动的方向数量
            int directions = 0;
            for (int i = 0; i < 8; i++) {
                int nx = piece.first + DIRECTIONS[i][0];
                int ny = piece.second + DIRECTIONS[i][1];
                
                if (isValidPosition(nx, ny) && board[nx][ny] == EMPTY) {
                    directions++;
                }
            }
            flexibility += directions;
        }
        
        return flexibility;
    }
    
    // 对当前局面进行评价的函数
    int evaluate(int player) const {
        if (isGameOver()) {
            // 如果直接输了的话就不用这样了，直接输出一个非常大的数让他直接嘎掉
            if (player == currentPlayer) return -999999;
            else return 999999;
        }
        
        int score = 0;
        
        // 1. 移动力评估（重要）
        int myMobility = calculateMobility(player);
        int opponentMobility = calculateMobility((player == BLACK) ? WHITE : BLACK);
        score += (myMobility - opponentMobility) * 10;
        
        // 2. 区域控制评估（使用BFS计算可到达的空地数量）
        int myTerritory = calculateTerritory(player);
        int opponentTerritory = calculateTerritory((player == BLACK) ? WHITE : BLACK);
        score += (myTerritory - opponentTerritory) * 5;
        
        // 3. 棋子位置评估（中心控制）
        score += evaluatePiecePosition(player);
        
        // 4. 棋子灵活性（每个棋子可移动的方向数量）
        score += evaluatePieceFlexibility(player) * 3;
        
        return score;
    }
    
    // 对走法排序（用于alpha-beta剪枝优化）
    void orderMoves(vector<Move>& moves) {
        vector<pair<int, Move>> scoredMoves;
        
        for (const auto& move : moves) {
            int score = 0;
            
            // 理论上向棋盘中间移动不容易被堵住
            int centerX = BOARD_SIZE / 2;
            int centerY = BOARD_SIZE / 2;
            
            int distanceBefore = abs(move.from_x - centerX) + abs(move.from_y - centerY);
            int distanceAfter = abs(move.to_x - centerX) + abs(move.to_y - centerY);
            
            if (distanceAfter < distanceBefore) {
                score += 20;
            }
            
            // 理论上把箭放在棋盘中间可以对对方造成最大的干扰
            int arrowDist = abs(move.arrow_x - centerX) + abs(move.arrow_y - centerY);
            score += (14 - arrowDist);  // 箭离中心越近越好
            
            scoredMoves.push_back({score, move});
        }
        
        // 按分数排序（降序）
        sort(scoredMoves.begin(), scoredMoves.end(),
             [](const pair<int, Move>& a, const pair<int, Move>& b) {
                 return a.first > b.first;
             });
        
        // 更新moves列表
        moves.clear();
        for (const auto& item : scoredMoves) {
            moves.push_back(item.second);
        }
    }
    
    // Minimax搜索
    pair<int, Move> minimax(int depth, int alpha, int beta, bool maximizingPlayer) {
        // <得分,走法>
        if (depth == 0 || isGameOver()) {
            return {evaluate(maximizingPlayer ? currentPlayer : (currentPlayer == BLACK ? WHITE : BLACK)),
                    Move(-1, -1, -1, -1, -1, -1)};
        }
        
        int originalPlayer = currentPlayer;
        vector<Move> moves = generateAllMoves(originalPlayer);
        
        // 对走法排序以优化剪枝
        orderMoves(moves);
        
        // 如果现在轮到自己，那么就要通过递归选出得分最大的走法
        if (maximizingPlayer) {
            int maxEval = -1000000;
            Move bestMove = moves.empty() ? Move(-1, -1, -1, -1, -1, -1) : moves[0];
            
            for (const auto& move : moves) {
                makeMove(move);
                int eval = minimax(depth - 1, alpha, beta, false).first;
                undoMove(move);
                
                if (eval > maxEval) {
                    maxEval = eval;
                    bestMove = move;
                }
                
                alpha = max(alpha, eval);
                if (beta <= alpha) {
                    break;  // Beta剪枝
                }
            }
            
            return {maxEval, bestMove};
        }
        
        // 如果现在是对手走，那么就要选出最小的走法
        else {
            int minEval = 1000000;
            Move bestMove = moves.empty() ? Move(-1, -1, -1, -1, -1, -1) : moves[0];
            
            for (const auto& move : moves) {
                makeMove(move);
                int eval = minimax(depth - 1, alpha, beta, true).first;
                undoMove(move);
                
                if (eval < minEval) {
                    minEval = eval;
                    bestMove = move;
                }
                
                beta = min(beta, eval);
                if (beta <= alpha) {
                    break;  // Alpha剪枝
                }
            }
            
            return {minEval, bestMove};
        }
    }
    
    // 获取最佳走法（主函数）
    Move getBestMove(int depth) {
        // 使用迭代加深搜索
        Move bestMove(-1, -1, -1, -1, -1, -1);
        int bestValue = -1000000;
        int originalPlayer = currentPlayer;
        
        for (int d = 1; d <= depth; d++) {
            auto result = minimax(d, -1000000, 1000000, true);
            
            if (result.first > bestValue) {
                bestValue = result.first;
                bestMove = result.second;
            }
            
            // 如果有必胜局面，直接返回
            if (bestValue > 9000) {
                break;
            }
        }
        
        currentPlayer = originalPlayer; // 恢复当前玩家
        return bestMove;
    }
    
    // 根据难度获取AI移动
    Move getAIMove() {
        switch (difficulty) {
            case EASY:
                return getRandomMove();
            case MEDIUM:
                return getBestMove(1);
            case HARD:
                return getBestMove(2);
            default:
                return getRandomMove();
        }
    }
    
    // 打印棋盘
    void printBoard() const {
        cout << "  ";
        for (int i = 0; i < BOARD_SIZE; i++) {
            cout << i << " ";
        }
        cout << endl;
        
        for (int y = 0; y < BOARD_SIZE; y++) {
            cout << y << " ";
            for (int x = 0; x < BOARD_SIZE; x++) {
                switch (board[x][y]) {
                    case EMPTY: cout << ". "; break;
                    case BLACK: cout << "B "; break;
                    case WHITE: cout << "W "; break;
                    case ARROW: cout << "X "; break;
                }
            }
            cout << endl;
        }
    }
    
    // 获取当前玩家
    int getCurrentPlayer() const {
        return currentPlayer;
    }
    
    // 设置当前玩家
    void setCurrentPlayer(int player) {
        currentPlayer = player;
    }
    
    // 获取棋盘字符
    char getBoardChar(int x, int y) const {
        switch (board[x][y]) {
            case EMPTY: return '.';
            case BLACK: return 'B';
            case WHITE: return 'W';
            case ARROW: return 'X';
            default: return '?';
        }
    }
    
    // 设置棋盘字符
    void setBoardChar(int x, int y, char c) {
        switch (c) {
            case '.': board[x][y] = EMPTY; break;
            case 'B': board[x][y] = BLACK; break;
            case 'W': board[x][y] = WHITE; break;
            case 'X': board[x][y] = ARROW; break;
        }
    }
    
    // 检查走法是否合法
    bool isValidMove(const Move& move) const {
        // 检查起点是否有当前玩家的棋子
        if (board[move.from_x][move.from_y] != currentPlayer) {
            return false;
        }
        
        // 检查终点是否为空
        if (board[move.to_x][move.to_y] != EMPTY) {
            return false;
        }
        
        // 检查箭的位置是否为空
        if (board[move.arrow_x][move.arrow_y] != EMPTY) {
            return false;
        }
        
        // 检查移动是否合法（皇后走法）
        int dx = move.to_x - move.from_x;
        int dy = move.to_y - move.from_y;
        
        if (dx == 0 && dy == 0) return false;  // 不能不动
        
        // 检查方向是否合法
        if (dx != 0 && dy != 0 && abs(dx) != abs(dy)) {
            return false;
        }
        
        // 检查路径是否畅通
        int stepX = (dx == 0) ? 0 : dx / abs(dx);
        int stepY = (dy == 0) ? 0 : dy / abs(dy);
        
        int x = move.from_x + stepX;
        int y = move.from_y + stepY;
        
        while (x != move.to_x || y != move.to_y) {
            if (!isValidPosition(x, y) || board[x][y] != EMPTY) {
                return false;
            }
            x += stepX;
            y += stepY;
        }
        
        // 检查放箭的合法性
        dx = move.arrow_x - move.to_x;
        dy = move.arrow_y - move.to_y;
        
        if (dx == 0 && dy == 0) {
            return false;  // 不能放在自己位置上
        }
        
        if (dx != 0 && dy != 0 && abs(dx) != abs(dy)) {
            return false;
        }
        
        // 检查箭的路径
        stepX = (dx == 0) ? 0 : dx / abs(dx);
        stepY = (dy == 0) ? 0 : dy / abs(dy);
        
        x = move.to_x + stepX;
        y = move.to_y + stepY;
        
        while (x != move.arrow_x || y != move.arrow_y) {
            if (!isValidPosition(x, y) || board[x][y] != EMPTY) {
                return false;
            }
            x += stepX;
            y += stepY;
        }
        
        return true;
    }
    
    // 保存游戏状态到文件
    bool saveGame(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "无法保存游戏！无法创建文件 " << filename << endl;
            return false;
        }
        
        // 保存棋盘状态
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                file << getBoardChar(x, y);
            }
            file << endl;
        }
        
        // 保存当前玩家
        file << currentPlayer << endl;
        
        // 保存难度
        file << static_cast<int>(difficulty) << endl;
        
        file.close();
        cout << "游戏已保存到 " << filename << endl;
        return true;
    }
    
    // 从文件加载游戏状态
    bool loadGame(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "无法加载游戏！无法打开文件 " << filename << endl;
            cout << "请先开始新游戏并保存一次。" << endl;
            return false;
        }
        
        // 检查文件是否为空
        file.seekg(0, ios::end);
        if (file.tellg() == 0) {
            cout << "存档文件为空！" << endl;
            file.close();
            return false;
        }
        file.seekg(0, ios::beg);
        
        // 加载棋盘状态
        vector<string> lines;
        string line;
        
        // 读取棋盘状态（前8行）
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (!getline(file, line)) {
                cout << "存档文件格式错误：缺少棋盘行" << endl;
                file.close();
                return false;
            }
            
            // 检查行长度
            if (line.length() < BOARD_SIZE) {
                cout << "存档文件格式错误：第" << i+1 << "行太短" << endl;
                file.close();
                return false;
            }
            
            lines.push_back(line);
        }
        
        // 应用棋盘状态
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                char c = lines[y][x];
                if (c == '.' || c == 'B' || c == 'W' || c == 'X') {
                    setBoardChar(x, y, c);
                } else {
                    cout << "存档文件包含无效字符 '" << c << "' 在位置 (" << x << "," << y << ")" << endl;
                    file.close();
                    return false;
                }
            }
        }
        
        // 重新构建棋子列表
        blackPieces.clear();
        whitePieces.clear();
        for (int x = 0; x < BOARD_SIZE; x++) {
            for (int y = 0; y < BOARD_SIZE; y++) {
                if (board[x][y] == BLACK) {
                    blackPieces.push_back({x, y});
                } else if (board[x][y] == WHITE) {
                    whitePieces.push_back({x, y});
                }
            }
        }
        
        // 检查棋子数量是否正确
        if (blackPieces.size() > 4 || whitePieces.size() > 4) {
            cout << "存档文件错误：棋子数量过多" << endl;
            cout << "黑棋: " << blackPieces.size() << "个, 白棋: " << whitePieces.size() << "个" << endl;
            file.close();
            return false;
        }
        
        // 加载当前玩家
        if (!(file >> currentPlayer)) {
            cout << "存档文件格式错误：无法读取当前玩家" << endl;
            file.close();
            return false;
        }
        
        if (currentPlayer != BLACK && currentPlayer != WHITE) {
            cout << "存档文件错误：当前玩家值无效" << endl;
            file.close();
            return false;
        }
        
        // 加载难度
        int diff;
        if (!(file >> diff)) {
            cout << "存档文件格式错误：无法读取难度设置" << endl;
            file.close();
            return false;
        }
        
        if (diff < 0 || diff > 2) {
            cout << "存档文件错误：难度设置无效" << endl;
            file.close();
            return false;
        }
        
        difficulty = static_cast<Difficulty>(diff);
        
        file.close();
        cout << "游戏已从 " << filename << " 成功加载" << endl;
        return true;
    }
};

// 游戏管理器类
class GameManager {
private:
    AmazonsGame game;
    GameState state;
    bool playerIsBlack;  // 玩家是否为黑棋
    bool gameStarted;    // 标记游戏是否已经开始
    string saveFilename = "amazon_save.txt";
    
public:
    GameManager() {
        state = MENU;
        gameStarted = false;
    }
    
    // 显示菜单
    void showMenu() {
        cout << "\n=== 亚马逊棋 ===" << endl;
        cout << "1. 新游戏" << endl;
        cout << "2. 保存游戏" << endl;
        cout << "3. 加载游戏" << endl;
        cout << "4. 退出游戏" << endl;
        cout << "请选择: ";
    }
    
    // 选择难度
    Difficulty selectDifficulty() {
        int choice;
        while (true) {
            cout << "\n选择难度:" << endl;
            cout << "1. 简单 (随机算法)" << endl;
            cout << "2. 中等 (AI算法，深度1)" << endl;
            cout << "3. 困难 (AI算法，深度2)" << endl;
            cout << "请选择 (1-3): ";
            cin >> choice;
            
            if (choice >= 1 && choice <= 3) {
                break;
            }
            cout << "无效选择，请重新输入！" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        switch (choice) {
            case 1: return EASY;
            case 2: return MEDIUM;
            case 3: return HARD;
            default: return MEDIUM;
        }
    }
    
    // 开始新游戏
    void startNewGame() {
        // 选择难度
        Difficulty diff = selectDifficulty();
        game.setDifficulty(diff);
        
        // 选择颜色
        char choice;
        while (true) {
            cout << "选择颜色 (B: 黑色先手 / W: 白色后手): ";
            cin >> choice;
            choice = toupper(choice);
            
            if (choice == 'B' || choice == 'W') {
                break;
            }
            cout << "无效选择，请重新输入！" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        playerIsBlack = (choice == 'B');
        
        game.initializeBoard();
        gameStarted = true;
        
        // 如果玩家选择白棋，AI先手
        if (!playerIsBlack) {
            game.setCurrentPlayer(1);  // 黑棋先手
            cout << "AI先手（黑色）" << endl;
        } else {
            cout << "你先手（黑色）" << endl;
        }
        
        state = PLAYING;
    }
    
    // 玩家移动
    void playerMove() {
        int fromX, fromY, toX, toY, arrowX, arrowY;
        bool validMove = false;
        
        while (!validMove) {
            cout << "请输入要移动的棋子坐标 (x y): ";
            if (!(cin >> fromX >> fromY)) {
                cout << "输入格式错误，请重新输入！" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            
            cout << "请输入移动目标坐标 (x y): ";
            if (!(cin >> toX >> toY)) {
                cout << "输入格式错误，请重新输入！" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            
            cout << "请输入箭的位置 (x y): ";
            if (!(cin >> arrowX >> arrowY)) {
                cout << "输入格式错误，请重新输入！" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            
            Move move(fromX, fromY, toX, toY, arrowX, arrowY);
            validMove = game.isValidMove(move);
            
            if (!validMove) {
                cout << "移动无效，请重新输入！" << endl;
            } else {
                game.makeMove(move);
                cout << "玩家移动: (" << fromX << "," << fromY << ") -> ("
                     << toX << "," << toY << "), 箭在("
                     << arrowX << "," << arrowY << ")" << endl;
            }
        }
    }
    
    // AI移动
    void aiMove() {
        cout << "AI正在思考..." << endl;
        Move aiMove = game.getAIMove();
        
        if (aiMove.isValid()) {
            game.makeMove(aiMove);
            cout << "AI移动: (" << aiMove.from_x << "," << aiMove.from_y << ") -> ("
                 << aiMove.to_x << "," << aiMove.to_y << "), 箭在("
                 << aiMove.arrow_x << "," << aiMove.arrow_y << ")" << endl;
        } else {
            cout << "AI没有合法移动，玩家获胜！" << endl;
            state = GAME_OVER;
        }
    }
    
    // 检查游戏是否结束
    bool checkGameOver() {
        if (game.isGameOver()) {
            int currentPlayer = game.getCurrentPlayer();
            if ((playerIsBlack && currentPlayer == 1) || (!playerIsBlack && currentPlayer == 2)) {
                cout << "玩家没有合法移动，AI获胜！" << endl;
            } else {
                cout << "AI没有合法移动，玩家获胜！" << endl;
            }
            return true;
        }
        return false;
    }
    
    // 显示当前棋盘和状态
    void displayGame() {
        cout << "\n当前棋盘:" << endl;
        game.printBoard();
        
        int currentPlayer = game.getCurrentPlayer();
        bool isPlayerTurn = (playerIsBlack && currentPlayer == 1) || (!playerIsBlack && currentPlayer == 2);
        
        cout << "当前回合: " << (isPlayerTurn ? "玩家" : "AI") << " (";
        cout << (currentPlayer == 1 ? "黑色" : "白色") << ")" << endl;
        
        // 显示当前难度
        Difficulty diff = game.getDifficulty();
        cout << "当前难度: ";
        switch (diff) {
            case EASY: cout << "简单"; break;
            case MEDIUM: cout << "中等"; break;
            case HARD: cout << "困难"; break;
        }
        cout << endl;
    }
    
    // 保存游戏
    void saveGame() {
        if (!gameStarted) {
            cout << "没有游戏可以保存！请先开始新游戏。" << endl;
            return;
        }
        
        if (game.saveGame(saveFilename)) {
            cout << "保存成功！" << endl;
        }
    }
    
    // 加载游戏
    void loadGame() {
        if (game.loadGame(saveFilename)) {
            gameStarted = true;
            // 由于存档中没有保存玩家颜色信息，这里假设玩家是黑棋
            // 或者可以询问用户
            playerIsBlack = true;
            state = PLAYING;
            cout << "游戏加载成功！" << endl;
            displayGame();
        }
    }
    
    // 主游戏循环
    void run() {
        srand(static_cast<unsigned int>(time(0)));
        
        while (true) {
            if (state == MENU) {
                showMenu();
                int choice;
                if (!(cin >> choice)) {
                    cout << "输入错误，请重新输入！" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                
                switch (choice) {
                    case 1:
                        startNewGame();
                        break;
                    case 2:
                        saveGame();
                        break;
                    case 3:
                        loadGame();
                        break;
                    case 4:
                        cout << "再见！" << endl;
                        return;
                    default:
                        cout << "无效选择！" << endl;
                        break;
                }
            }
            else if (state == PLAYING) {
                displayGame();
                
                if (checkGameOver()) {
                    state = GAME_OVER;
                    continue;
                }
                
                int currentPlayer = game.getCurrentPlayer();
                bool isPlayerTurn = (playerIsBlack && currentPlayer == 1) || (!playerIsBlack && currentPlayer == 2);
                
                if (isPlayerTurn) {
                    // 玩家回合
                    cout << "\n玩家回合" << endl;
                    cout << "1. 走棋" << endl;
                    cout << "2. 返回菜单" << endl;
                    cout << "3. 保存游戏" << endl;
                    cout << "请选择: ";
                    
                    int choice;
                    if (!(cin >> choice)) {
                        cout << "输入错误，请重新输入！" << endl;
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }
                    
                    if (choice == 1) {
                        playerMove();
                    } else if (choice == 2) {
                        state = MENU;
                    } else if (choice == 3) {
                        saveGame();
                    } else {
                        cout << "无效选择！" << endl;
                    }
                } else {
                    // AI回合
                    cout << "\nAI回合" << endl;
                    aiMove();
                    
                    // 暂停一下让玩家看到AI的移动
                    if (state == PLAYING) {
                        cout << "按回车继续...";
                        cin.ignore();
                        cin.get();
                    }
                }
            }
            else if (state == GAME_OVER) {
                cout << "\n游戏结束！" << endl;
                displayGame();
                
                cout << "\n1. 新游戏" << endl;
                cout << "2. 保存游戏" << endl;
                cout << "3. 返回菜单" << endl;
                cout << "4. 退出" << endl;
                cout << "请选择: ";
                
                int choice;
                if (!(cin >> choice)) {
                    cout << "输入错误，请重新输入！" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                
                if (choice == 1) {
                    startNewGame();
                } else if (choice == 2) {
                    saveGame();
                } else if (choice == 3) {
                    state = MENU;
                } else if (choice == 4) {
                    cout << "再见！" << endl;
                    return;
                } else {
                    cout << "无效选择！" << endl;
                }
            }
        }
    }
};

int main() {
    GameManager manager;
    manager.run();
    return 0;
}
