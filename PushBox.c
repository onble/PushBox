#include <stdio.h> //C语言标准库

#include <windows.h> //设置黑窗口的属性依赖的库
/*依赖此库的函数：
    void HideCursor();//隐藏光标闪烁
    void gotoxy(int x, int y)//按坐标设置printf输出位置
*/

#include <conio.h>
//键盘输入需要的函数

//这个库似乎被coin库包含了
// #include <stdlib.h>
//修改滚动条问题

//设置最大关卡数
#define MaxLevel 20

enum
{
    UP = 72,
    DOWN = 80,
    LEFT = 75,
    RIGHT = 77,
    ESC = 27,
    PAUSE = 32
}; //设置键盘的枚举类型

//记录每一步信息的节点
//因为要有后悔一步，所以要用头插法
typedef struct _INFO
{
    int direction;
    /*下面定义不同参数代表的含义
        0:原点，不可以再后退
        1：上路
        2：下路
        3：左路
        4：右路
        11：向上推箱子，再前面为路
        12：向下推箱子，再前面为路
        13：向左推箱子，再前面为路
        14：向右推箱子，再前面为路
        21：向上推箱子，再前面为目标点
        22：向下推箱子，再前面为目标点
        23：向左推箱子，再前面为目标点
        24：向右推箱子，再前面为目标点
        31：向上推绿箱子，再前面为路
        32：向下推绿箱子，再前面为路
        33：向左推绿箱子，再前面为路
        34：向右推绿箱子，再前面为路
        41：向上推绿箱子，再前面为目标点
        42：向下推绿箱子，再前面为目标点
        43：向左推绿箱子，再前面为目标点
        44：向右推绿箱子，再前面为目标点
        51：上目标点
        52：下目标点
        53：左目标点
        54：右目标点
        通过通用各位可以在重复演示的时候只需要个位便能够与move部分的函数进行结合，从而更方便演示
    */
    struct _INFO *next;
} Info_move;
//这种定义方法真巧妙

//打开欢迎界面
void Welcome();

//获取主页面的输入
int ChoiceInMenu();

//定义隐藏光标函数
//此函数依赖windows.h库
void HideCursor(int flag)
{
    CONSOLE_CURSOR_INFO cursor;
    if (flag == 0)
    {
        cursor.bVisible = FALSE;
    }
    else
    {
        cursor.bVisible = TRUE;
    }
    cursor.dwSize = sizeof(cursor);
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(handle, &cursor);
}

//设置光标位置的函数
void gotoxy(int x, int y);

int StartGame(int choiceInMenu);

//创建地图
int CreateMap(int level, Info_move *schedule);

//打印单元格
void PrintMate(int kind);

//检查是否通关
//此函数被取消使用
// int CheackOver(int **mapMatrix, int xSize, int ySize);

//记录移动方向函数
void RecordMoveInfo(Info_move *chainHead, int direction);

//绘制提示信息
void PrintInfo(int level);

//改写步数
void PrintStepNumber(int stepNumber);

//保存行动数据
void SaveData(Info_move *head, int level);

//加载进度
Info_move *LoadingSchedule(int level);

//打印保存中
void PrintSaving(Info_move *head, int level);

//保存关卡进度
void SaveLevelToFile(int level);

int main()
{
    //设置黑窗口的界面大小
    system("mode con cols=90 lines=45"); // cols是列,一列占10像素,lines是行,一行占20像素

    //设置控制台的标题
    SetConsoleTitle("PushBox");

    //隐藏光标在黑窗口显示，减少不停的闪烁
    HideCursor(0);

    //这里可以放加载页面（速度要快那种)
    gotoxy(0, 15);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 124);
    printf("■■■■                                            ");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);
    printf("■\n");
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 124);
    printf("■■■■                                            ");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);
    printf("■\n");
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);
    Sleep(300);
    gotoxy(0, 15);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 126);
    printf("■■■■■■■■■■■■■■■■■                  ");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);
    printf("■\n");
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 126);
    printf("■■■■■■■■■■■■■■■■■                  ");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);
    printf("■\n");
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);
    // Sleep(3000);

    //若此行编译报错，请参考  https://blog.csdn.net/JACK__Q/article/details/107182729
    //或  https://www.jianshu.com/p/0115c67132c9
    // D:\\programming\\test\\C\\programBase\\gluttonoueSnake\\MyMusic.wav
    // PlaySound(TEXT("programBase\\gluttonoueSnake\\MyMusic.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    mciSendString("open Music/MenuMusic.mp3 alias MenuMusic", NULL, 0, NULL);
    mciSendString("open Music/BackgroundMusic.mp3 alias BackgroundMusic", NULL, 0, NULL);
    // mciSendString("open Music/WHOOSH.mp3 alias PushBoxing", NULL, 0, NULL);

    gotoxy(0, 15);
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 122);
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);
    printf("■\n");
    printf("■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 122);
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);
    printf("■\n");
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
    Sleep(500);

    //设置字体大小
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof cfi;
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 15; // 10是默认值
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

    //定义变量一定要在函数最外层，也就是只有一个缩进
    // 1代表:从levbel 1开始游戏
    // 2代表,想要返回菜单页
    // 3代表:想要进入特定level
    //-1代表重复演示
    int choice;

    //进入主界面
    while (1)
    {
        Welcome();

        // 1代表:从levbel 1开始游戏
        // 2代表,想要返回菜单页
        // 3代表:想要进入特定level
        //-1代表重复演示
        //-2代表继续游戏
        //获取主页面的输入
        choice = ChoiceInMenu();
        switch (choice)
        {
        case 1:
            // 1代表从level 1 开始游戏
            SaveLevelToFile(1);
            choice = StartGame(1);
            break; // break一定不要忘记,要先写break;
        case -1:
            choice = StartGame(-1);
            break;
        case 2:
            //想要返回菜单页
            break;
        case -2:
            //-2代表继续游戏
            choice = StartGame(-2);
            break;
        case 3:
            //用户想要进入特定level
            choice = StartGame(3);
            break;
        }
    }

    system("pause>nul"); //防止程序立刻退出，完工前需要优化//回车键无效
}

//设置光标位置的函数
void gotoxy(int x, int y)
{
    COORD pos = {x, y};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, pos);
}

//打开欢迎界面
void Welcome()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY); //清理画笔颜色
    //清屏
    system("cls");

    //控制留多少空行
    for (int i = 0; i < 10; i++)
    {
        printf("\n");
    }

    //打印标题
    //生成网址:http://patorjk.com/software/taag/#p=display&f=Big%20Money-ne&t=PushBox%0A
    HANDLE firstPart_PucshBox = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(firstPart_PucshBox, FOREGROUND_INTENSITY | FOREGROUND_RED); //设置红色字体
    printf("\t");
    printf(" /$$$$$$$                      /$$       /$$$$$$$                     \n");
    printf("\t");
    printf("| $$__  $$                    | $$      | $$__  $$                    \n");
    printf("\t");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN); //设置绿色字体

    printf("| $$  \\ $$ /$$   /$$  /$$$$$$$| $$$$$$$ | $$  \\ $$  /$$$$$$  /$$   /$$\n");
    printf("\t");
    printf("| $$$$$$$/| $$  | $$ /$$_____/| $$__  $$| $$$$$$$  /$$__  $$|  $$ /$$/\n");
    printf("\t");
    printf("| $$____/ | $$  | $$|  $$$$$$ | $$  \\ $$| $$__  $$| $$  \\ $$ \\  $$$$/ \n");
    printf("\t");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE); //设置蓝色字体

    printf("| $$      | $$  | $$ \\____  $$| $$  | $$| $$  \\ $$| $$  | $$  >$$  $$ \n");
    printf("\t");
    printf("| $$      |  $$$$$$/ /$$$$$$$/| $$  | $$| $$$$$$$/|  $$$$$$/ /$$/\\  $$\n");
    printf("\t");
    printf("|__/       \\______/ |_______/ |__/  |__/|_______/  \\______/ |__/  \\__/\n");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY); //设置颜色，没有添加颜色，故为原色

    //控制留多少空行
    for (int i = 0; i < 5; i++)
    {
        printf("\n");
    }

    printf("\t\t=========================================================\n");
    printf("\t\t=                                                       =\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | BACKGROUND_GREEN);
    printf("\t\t=              Press [P] to start New game              =\n");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY); //设置颜色，没有添加颜色，故为原色
    printf("\t\t=              [E]xit                                   =\n");
    printf("\t\t=              [S]elect level                           =\n");
    printf("\t\t=              [C]ontinue                               =\n");
    printf("\t\t=                                                       =\n");
    printf("\t\t=                                                       =\n");
    printf("\t\t=========================================================\n");

    //播放背景音乐
    mciSendString("play MenuMusic repeat", NULL, 0, NULL);
}

//获取主页面的输入
// 1代表开始游戏
//-1用户想要继续游戏
// 3用户想要进入特定level
int ChoiceInMenu()
{
    while (1)
    {
        switch (getch())
        {
        case 'p':
        case 'P':
            return 1; // 1代表开始游戏
            break;
        case 'E':
        case 'e':
            exit(0);
        case 'C':
        case 'c':
            //用户想要继续游戏
            return -2;
            break;
        case 'S':
        case 's':
            //用户想要进入特定level
            return 3;
            break;
        }
    }
}

//开始游戏
//返回2,代表想要回到菜单页
//传矩阵被警告,所以move函数要放到CreatMap函数里面了
int StartGame(int choiceInMenu)
{
    int flag = 0;
    int level = 1;
    Info_move *schedule = NULL;

    //判断是否是想继续游戏
    int inputLevel = 1;
    if (choiceInMenu == -2)
    {
        //读入上次游玩的关卡
        FILE *inputLevelFromFile;
        if ((inputLevelFromFile = fopen("information\\mainInf.txt", "r")) == NULL)
        {
            printf("ERROR to read level from mainInf.txt file");
        }
        fscanf(inputLevelFromFile, "%d", &inputLevel);
        schedule = LoadingSchedule(inputLevel);
        fclose(inputLevelFromFile);
        level = inputLevel;
    }
    else
    {
        if (choiceInMenu == 3)
        {
            //用户想要进入特定level
            //打印输入框
            gotoxy(0, 23);
            printf("\t\t=========================================================\n");
            printf("\t\t=                                                       =\n");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | BACKGROUND_GREEN);
            printf("\t\t=              please inputthe level                    =\n");

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY); //设置颜色，没有添加颜色，故为原色
            printf("\t\t=               --------                                =\n");
            printf("\t\t=              │       │                                =\n");
            printf("\t\t=               --------                                =\n");
            printf("\t\t=                                                       =\n");
            printf("\t\t=                                                       =\n");
            printf("\t\t=              Maxlevel:%d                               =\n", MaxLevel);
            printf("\t\t=========================================================\n");

            //显示鼠标
            HideCursor(1);
            //定位鼠标
            gotoxy(34, 27);
            while (1)
            {
                fflush(stdin); /*清除输入缓冲区*/
                //万恶的缓存区啊啊啊啊
                scanf("%d", &inputLevel);
                if (inputLevel > 0 && inputLevel < MaxLevel)
                {
                    level = inputLevel;

                    //保存进度
                    SaveLevelToFile(level);

                    //隐藏鼠标
                    HideCursor(0);

                    //清除进度链条
                    // schedule=NULL;
                    break;
                }
            }
        }
    }

    while (1)
    {
        flag = CreateMap(level, schedule);
        if (flag == 0)
        {
            level++;
        }
        if (flag == 2)
        {
            //此时返回值说明想返回菜单
            return 2;
        }
        if (flag == 3)
        {
            //此时说明想要重复演示
            schedule = LoadingSchedule(level);
        }
        else
        {
            schedule = NULL;
        }
    }
}

//创建地图
//返回值为0表示正确通关
//返回值为1代表重新开始
//返回值为2代表想要回菜单页
//返回值为3代表想要重复演示
int CreateMap(int level, Info_move *schedule)
{
    //清屏
    system("cls");

    //设置字体大小
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof cfi;
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 40; // 10是默认值
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

    //重设窗口大小
    //设置黑窗口的界面大小
    system("mode con cols=60 lines=20"); // cols是列,一列占10像素,lines是行,一行占20像素

    //加工地图文件的名字
    char mapFileName[21] = {"map\\PushBoxMap00.txt"}; //注意大小啊啊啊啊啊
    mapFileName[14] = level / 10 + '0';
    mapFileName[15] = level % 10 + '0';

    int xSize, ySize;
    FILE *map; //地图文件指针
    if ((map = fopen(mapFileName, "r")) == NULL)
    {
        // perror("fail to read"); //此处需维护
        printf("%s\n", mapFileName);
        printf("error to find map");
        // exit(1);
        //此处应该return一个值表示状态
    }
    else
    {
        //开始读入数据并绘制地图
        fscanf(map, "%d %d", &xSize, &ySize);
        int mapMatrix[xSize][ySize]; // C99标准开始支持变量定义数组大小了
        for (int i = 0; i < xSize; i++)
        {
            for (int j = 0; j < ySize; j++)
            {
                fscanf(map, "%d", &mapMatrix[i][j]);
            }

            //下面这行可以不用写
            // fscanf(map, "%c");
        }

        //读入图像位置偏差
        int offset_vertical, offset_crosswise_prscise;
        fscanf(map, "%d %d", &offset_vertical, &offset_crosswise_prscise);

        //读完数据后随手关掉文件流
        fclose(map);
        //绘制地图

        //绘制上方留白
        for (int i = 0; i < offset_vertical; i++)
        {
            printf("\n");
        }
        for (int i = 0; i < xSize; i++)
        {
            //绘制左侧留白
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY); //清除画笔
            for (int blank = 0; blank < offset_crosswise_prscise; blank++)
            {
                printf(" ");
            }
            for (int j = 0; j < ySize; j++)
            {
                PrintMate(mapMatrix[i][j]);
            }
            printf("\n");
        }
        //绘制地图完毕

        //绘制提示信息
        PrintInfo(level);
        //下面控制音乐播放
        //首先停止菜单页的音乐播放
        mciSendString("stop MenuMusic", NULL, 0, NULL);
        //然后循环播放背景音乐
        mciSendString("play BackgroundMusic repeat", NULL, 0, NULL);

        //注意一下x,y轴到底是哪一个

        //找到人的位置
        int people_vertical_x, people_crosswise_y;
        for (int i = 1; i < xSize; i++)
        {
            for (int j = 1; j < ySize; j++)
            {
                if (mapMatrix[i][j] == 8)
                {
                    people_vertical_x = i;
                    people_crosswise_y = j;
                }
            }
        }

        //建立一个链表，记录操作
        //因为需要重复演示，所以每一个动作都要记录
        //可以记录每一步到下一步的时间，但并没有多大意义，相当于看自己玩一遍，所以不记录
        //创建信息指针
        Info_move *chain_head = (Info_move *)malloc(sizeof(Info_move));
        Info_move *chain_rear = (Info_move *)malloc(sizeof(Info_move));
        chain_rear->direction = 0;
        chain_rear->next = NULL;
        chain_head->direction = 0; //利用头域记录步数
        chain_head->next = chain_rear;
        //初始化链表头结束

        // switch语言中不能定义变量
        Info_move *cheack;
        // repeat指针用作重复动画的指针
        Info_move *repeat = chain_head;
        //读取schedule参数的指针
        Info_move *input_number;
        if (schedule != NULL)
        {
            input_number = schedule->next;

            //打印提示重复中
            gotoxy(40, 12);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 188);
            printf("repeating!");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY); //消除画笔
        }

        //给在祝贺页面选择N，下一关预留的参数
        char filename_forClean[30] = {"information\\ActionOf00.txt"};
        FILE *file_forClean;
        // COORD size = {90, 45};//设置缓冲区域的参数

        //定义记录步数的变量
        // int stepNumber = 0;
        //记录在了链表当中

        //创建空载的action文件
        SaveData(NULL, level);

        //下面开始动
        char input;
        int flag_conduct = 1;
        while (flag_conduct > 0)
        {
            if (schedule == NULL)
            {
                input = getch();
            }
            else
            {
                //设置自动输入延迟
                Sleep(150);
                switch (input_number->direction % 10)
                {
                case 1:
                    input = UP;
                    break;
                case 2:
                    input = DOWN;
                    break;
                case 3:
                    input = LEFT;
                    break;
                case 4:
                    input = RIGHT;
                    break;
                default:
                    // printf("ERRPR:wrong data from input file to input");
                    break;
                }
                input_number = input_number->next;
                if (input_number->next == NULL)
                {
                    free(schedule);
                    schedule = NULL;

                    //清除repeating提示字样
                    gotoxy(40, 12);
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
                    printf("                   ");
                }
            }
            switch (input)
            {
            case UP:
            case 'W':
            case 'w':
                if (mapMatrix[people_vertical_x - 1][people_crosswise_y] == 2)
                {
                    //前面为道路
                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                    PrintMate(8); //更改显示人物位置
                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                    PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                    mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                    people_vertical_x--;                                  //更改人物位置信息
                    mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵

                    //下面记录移动信息
                    RecordMoveInfo(chain_head, 1);

                    //控制走在路上的音效
                    PlaySound(TEXT("Music\\ARROW.wav"), NULL, SND_FILENAME | SND_ASYNC);
                }
                else
                {
                    if (mapMatrix[people_vertical_x - 1][people_crosswise_y] == 4)
                    {
                        //前面为箱子
                        if (mapMatrix[people_vertical_x - 2][people_crosswise_y] == 2) //检查更上面的单元
                        {
                            //前面为箱子,再前面为道路
                            gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                            PrintMate(8); //更改显示人物位置
                            gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 2);
                            PrintMate(4); //更改显示箱子位置
                            gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                            PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                            people_vertical_x--;                                  //更改人物位置信息
                            mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵
                            mapMatrix[people_vertical_x - 1][people_crosswise_y] = 4;

                            //下面记录移动信息
                            RecordMoveInfo(chain_head, 11);

                            //控制播放推箱子中的音效
                            // mciSendString("play PushBoxing", NULL, 0, NULL);
                            PlaySound(TEXT("Music\\WHOOSH.wav"), NULL, SND_FILENAME | SND_ASYNC);
                        }
                        else
                        {
                            //检查是否推到目标位置上面
                            if (mapMatrix[people_vertical_x - 2][people_crosswise_y] == 3)
                            {
                                //前面为箱子,再前面为目标点
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                                PrintMate(8); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 2);
                                PrintMate(7); //更改显示箱子位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(2); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = 2;
                                people_vertical_x--;                                  //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵
                                mapMatrix[people_vertical_x - 1][people_crosswise_y] = 7;

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 21);

                                //控制播放推箱子上目标点的音效
                                // mciSendString("play PushBoxing", NULL, 0, NULL);
                                PlaySound(TEXT("Music\\add_favorite.wav"), NULL, SND_FILENAME | SND_ASYNC);

                                //此时应该检验一下是否通关
                                int flag_GameOver = 1;
                                for (int i = 0; i < xSize; i++)
                                {
                                    for (int j = 0; j < ySize; j++)
                                    {
                                        if (mapMatrix[i][j] == 4)
                                        {
                                            flag_GameOver = 0;
                                        }
                                    }
                                }
                                if (flag_GameOver == 1)
                                {
                                    flag_conduct = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        //前面为绿箱子
                        if (mapMatrix[people_vertical_x - 1][people_crosswise_y] == 7)
                        {
                            //前面为绿箱子，再前面为道路
                            if (mapMatrix[people_vertical_x - 2][people_crosswise_y] == 2) //检查更上面的单元
                            {
                                //推动箱子
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                                PrintMate(9); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 2);
                                PrintMate(4); //更改显示箱子位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_vertical_x--;                                  //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵
                                mapMatrix[people_vertical_x - 1][people_crosswise_y] = 4;

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 31);

                                //控制播放推箱子离开目标点的音效
                                // mciSendString("play PushBoxing", NULL, 0, NULL);
                                PlaySound(TEXT("Music\\activation_change_fail.wav"), NULL, SND_FILENAME | SND_ASYNC);
                            }
                            else
                            {
                                //前面为箱子,再前面为目标点
                                if (mapMatrix[people_vertical_x - 2][people_crosswise_y] == 3)
                                {
                                    //此时前方为绿箱子，再往前为目标点
                                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                                    PrintMate(9); //更改显示人物位置
                                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 2);
                                    PrintMate(7); //更改显示箱子位置
                                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                    PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                    mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                    people_vertical_x--;                                  //更改人物位置信息
                                    mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵
                                    mapMatrix[people_vertical_x - 1][people_crosswise_y] = 7;

                                    //下面记录移动信息
                                    RecordMoveInfo(chain_head, 41);

                                    //控制播放推箱子上目标点的音效
                                    // mciSendString("play PushBoxing", NULL, 0, NULL);
                                    PlaySound(TEXT("Music\\add_favorite.wav"), NULL, SND_FILENAME | SND_ASYNC);
                                }
                            }
                        }
                        else
                        {
                            //前面为目标点
                            if (mapMatrix[people_vertical_x - 1][people_crosswise_y] == 3)
                            {
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                                PrintMate(9); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_vertical_x--;                                  //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 51);
                            }
                        }
                    }
                }
                break;
            case DOWN:
            case 'S':
            case 's':
                if (mapMatrix[people_vertical_x + 1][people_crosswise_y] == 2)
                {
                    //前方为道路
                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                    PrintMate(8); //更改显示人物位置
                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                    PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                    mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                    people_vertical_x++;                                  //更改人物位置信息
                    mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵

                    //下面记录移动信息
                    RecordMoveInfo(chain_head, 2);

                    //控制走在路上的音效
                    PlaySound(TEXT("Music\\ARROW.wav"), NULL, SND_FILENAME | SND_ASYNC);
                }
                else
                {
                    if (mapMatrix[people_vertical_x + 1][people_crosswise_y] == 4)
                    {
                        //前方为箱子
                        if (mapMatrix[people_vertical_x + 2][people_crosswise_y] == 2) //检查更下面的单元
                        {
                            //前方为箱子,再前方为道路
                            gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                            PrintMate(8); //更改显示人物位置
                            gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 2);
                            PrintMate(4); //更改显示箱子位置
                            gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                            PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                            people_vertical_x++;                                  //更改人物位置信息
                            mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵
                            mapMatrix[people_vertical_x + 1][people_crosswise_y] = 4;

                            //下面记录移动信息
                            RecordMoveInfo(chain_head, 12);

                            //控制播放推箱子中的音效
                            // mciSendString("play PushBoxing", NULL, 0, NULL);
                            PlaySound(TEXT("Music\\WHOOSH.wav"), NULL, SND_FILENAME | SND_ASYNC);
                        }
                        else
                        {
                            if (mapMatrix[people_vertical_x + 2][people_crosswise_y] == 3)
                            {
                                //前方为箱子,再前方为目标点
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                                PrintMate(8); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 2);
                                PrintMate(7); //更改显示箱子位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_vertical_x++;                                  //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵
                                mapMatrix[people_vertical_x + 1][people_crosswise_y] = 7;

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 22);

                                //控制播放推箱子上目标点的音效
                                // mciSendString("play PushBoxing", NULL, 0, NULL);
                                PlaySound(TEXT("Music\\add_favorite.wav"), NULL, SND_FILENAME | SND_ASYNC);

                                //此时应该检验一下是否通关
                                int flag_GameOver = 1;
                                for (int i = 0; i < xSize; i++)
                                {
                                    for (int j = 0; j < ySize; j++)
                                    {
                                        if (mapMatrix[i][j] == 4)
                                        {
                                            flag_GameOver = 0;
                                        }
                                    }
                                }
                                if (flag_GameOver == 1)
                                {
                                    flag_conduct = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        //前方为绿箱子
                        if (mapMatrix[people_vertical_x + 1][people_crosswise_y] == 7)
                        {
                            //前面为绿箱子,再往前为道路
                            if (mapMatrix[people_vertical_x + 2][people_crosswise_y] == 2) //检查更上面的单元
                            {
                                //此时是前方为绿箱子,再往前为道路
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                                PrintMate(9); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 2);
                                PrintMate(4); //更改显示箱子位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_vertical_x++;                                  //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵
                                mapMatrix[people_vertical_x + 1][people_crosswise_y] = 4;

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 32);

                                //控制播放推箱子离开目标点的音效
                                // mciSendString("play PushBoxing", NULL, 0, NULL);
                                PlaySound(TEXT("Music\\activation_change_fail.wav"), NULL, SND_FILENAME | SND_ASYNC);
                            }
                            else
                            {
                                if (mapMatrix[people_vertical_x + 2][people_crosswise_y] == 3) //检查更上面的单元
                                {
                                    //推动箱子
                                    //此时是前方为绿箱子,再往前为目标
                                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                                    PrintMate(9); //更改显示人物位置
                                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 2);
                                    PrintMate(7); //更改333显示箱子位置
                                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                    PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                    mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                    people_vertical_x++;                                  //更改人物位置信息
                                    mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵
                                    mapMatrix[people_vertical_x + 1][people_crosswise_y] = 7;

                                    //下面记录移动信息
                                    RecordMoveInfo(chain_head, 42);

                                    //控制播放推箱子上目标点的音效
                                    // mciSendString("play PushBoxing", NULL, 0, NULL);
                                    PlaySound(TEXT("Music\\add_favorite.wav"), NULL, SND_FILENAME | SND_ASYNC);
                                }
                            }
                        }
                        else
                        {
                            //前方为目标点
                            if (mapMatrix[people_vertical_x + 1][people_crosswise_y] == 3)
                            {
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                                PrintMate(9); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_vertical_x++;                                  //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 52);
                            }
                        }
                    }
                }
                break;
            case LEFT:
            case 'A':
            case 'a':
                if (mapMatrix[people_vertical_x][people_crosswise_y - 1] == 2)
                {
                    //前方为道路
                    gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                    PrintMate(8); //更改显示人物位置
                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                    PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                    mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                    people_crosswise_y--;                                 //更改人物位置信息
                    mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵
                    //下面记录移动信息
                    RecordMoveInfo(chain_head, 3);

                    //控制走在路上的音效
                    PlaySound(TEXT("Music\\ARROW.wav"), NULL, SND_FILENAME | SND_ASYNC);
                }
                else
                {
                    //前方为箱子
                    if (mapMatrix[people_vertical_x][people_crosswise_y - 1] == 4)
                    {
                        if (mapMatrix[people_vertical_x][people_crosswise_y - 2] == 2) //检查更左面的单元
                        {
                            //前方为箱子,再前方为道路
                            gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                            PrintMate(8); //更改显示人物位置
                            gotoxy(offset_crosswise_prscise + (people_crosswise_y - 2) * 2, offset_vertical + people_vertical_x);
                            PrintMate(4); //更改显示箱子位置
                            gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                            PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                            people_crosswise_y--;                                 //更改人物位置信息
                            mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵
                            mapMatrix[people_vertical_x][people_crosswise_y - 1] = 4;

                            //下面记录移动信息
                            RecordMoveInfo(chain_head, 13);

                            //控制播放推箱子中的音效
                            // mciSendString("play PushBoxing", NULL, 0, NULL);
                            PlaySound(TEXT("Music\\WHOOSH.wav"), NULL, SND_FILENAME | SND_ASYNC);
                        }
                        else
                        {
                            //前方为箱子，再前方为目标点
                            if (mapMatrix[people_vertical_x][people_crosswise_y - 2] == 3)
                            {
                                //推动箱子
                                gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                                PrintMate(8); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + (people_crosswise_y - 2) * 2, offset_vertical + people_vertical_x);
                                PrintMate(7); //更改显示箱子位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_crosswise_y--;                                 //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵
                                mapMatrix[people_vertical_x][people_crosswise_y - 1] = 7;

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 23);

                                //控制播放推箱子上目标点的音效
                                // mciSendString("play PushBoxing", NULL, 0, NULL);
                                PlaySound(TEXT("Music\\add_favorite.wav"), NULL, SND_FILENAME | SND_ASYNC);

                                //此时应该检验一下是否通关
                                int flag_GameOver = 1;
                                for (int i = 0; i < xSize; i++)
                                {
                                    for (int j = 0; j < ySize; j++)
                                    {
                                        if (mapMatrix[i][j] == 4)
                                        {
                                            flag_GameOver = 0;
                                        }
                                    }
                                }
                                if (flag_GameOver == 1)
                                {
                                    flag_conduct = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        //前方为绿箱子
                        if (mapMatrix[people_vertical_x][people_crosswise_y - 1] == 7)
                        {
                            if (mapMatrix[people_vertical_x][people_crosswise_y - 2] == 2) //检查更上面的单元
                            {
                                //此时是前方为绿箱子,再往前为道路
                                gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                                PrintMate(9); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + (people_crosswise_y - 2) * 2, offset_vertical + people_vertical_x);
                                PrintMate(4); //更改显示箱子位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_crosswise_y--;                                 //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵
                                mapMatrix[people_vertical_x][people_crosswise_y - 1] = 4;

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 33);

                                //控制播放推箱子离开目标点的音效
                                // mciSendString("play PushBoxing", NULL, 0, NULL);
                                PlaySound(TEXT("Music\\activation_change_fail.wav"), NULL, SND_FILENAME | SND_ASYNC);
                            }
                            else
                            {
                                if (mapMatrix[people_vertical_x][people_crosswise_y - 2] == 3) //检查更上面的单元
                                {
                                    //此时是前方为绿箱子,再往前为目标点
                                    gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                                    PrintMate(9); //更改显示人物位置
                                    gotoxy(offset_crosswise_prscise + (people_crosswise_y - 2) * 2, offset_vertical + people_vertical_x);
                                    PrintMate(7); //更改显示箱子位置
                                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                    PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                    mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                    people_crosswise_y--;                                 //更改人物位置信息
                                    mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵
                                    mapMatrix[people_vertical_x][people_crosswise_y - 1] = 7;

                                    //下面记录移动信息
                                    RecordMoveInfo(chain_head, 43);

                                    //控制播放推箱子上目标点的音效
                                    // mciSendString("play PushBoxing", NULL, 0, NULL);
                                    PlaySound(TEXT("Music\\add_favorite.wav"), NULL, SND_FILENAME | SND_ASYNC);
                                }
                            }
                        }
                        else
                        {
                            //前方为目标点
                            if (mapMatrix[people_vertical_x][people_crosswise_y - 1] == 3)
                            {
                                gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                                PrintMate(9); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_crosswise_y--;                                 //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 53);
                            }
                        }
                    }
                }
                break;
            case RIGHT:
            case 'D':
            case 'd':
                if (mapMatrix[people_vertical_x][people_crosswise_y + 1] == 2)
                {
                    //前方为道路
                    gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                    PrintMate(8); //更改显示人物位置
                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                    PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                    mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                    people_crosswise_y++;                                 //更改人物位置信息
                    mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵

                    //下面记录移动信息
                    RecordMoveInfo(chain_head, 4);

                    //控制走在路上的音效
                    PlaySound(TEXT("Music\\ARROW.wav"), NULL, SND_FILENAME | SND_ASYNC);
                }
                else
                {
                    //前方为箱子
                    if (mapMatrix[people_vertical_x][people_crosswise_y + 1] == 4)
                    {
                        //前方为箱子,再前方为道路
                        if (mapMatrix[people_vertical_x][people_crosswise_y + 2] == 2) //检查更右面的单元
                        {
                            //推动箱子
                            gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                            PrintMate(8); //更改显示人物位置
                            gotoxy(offset_crosswise_prscise + (people_crosswise_y + 2) * 2, offset_vertical + people_vertical_x);
                            PrintMate(4); //更改显示箱子位置
                            gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                            PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                            people_crosswise_y++;                                 //更改人物位置信息
                            mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵
                            mapMatrix[people_vertical_x][people_crosswise_y + 1] = 4;

                            //下面记录移动信息
                            RecordMoveInfo(chain_head, 14);

                            //控制播放推箱子中的音效
                            // mciSendString("play PushBoxing", NULL, 0, NULL);
                            PlaySound(TEXT("Music\\WHOOSH.wav"), NULL, SND_FILENAME | SND_ASYNC);
                        }
                        else
                        {
                            //前方为箱子,再前方为目标点
                            if (mapMatrix[people_vertical_x][people_crosswise_y + 2] == 3) //检查更右面的单元
                            {
                                //推动箱子
                                gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                                PrintMate(8); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + (people_crosswise_y + 2) * 2, offset_vertical + people_vertical_x);
                                PrintMate(7); //更改显示箱子位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_crosswise_y++;                                 //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 8; //修正矩阵
                                mapMatrix[people_vertical_x][people_crosswise_y + 1] = 7;

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 24);

                                //控制播放推箱子上目标点的音效
                                // mciSendString("play PushBoxing", NULL, 0, NULL);
                                PlaySound(TEXT("Music\\add_favorite.wav"), NULL, SND_FILENAME | SND_ASYNC);

                                //此时应该检验一下是否通关
                                int flag_GameOver = 1;
                                for (int i = 0; i < xSize; i++)
                                {
                                    for (int j = 0; j < ySize; j++)
                                    {
                                        if (mapMatrix[i][j] == 4)
                                        {
                                            flag_GameOver = 0;
                                        }
                                    }
                                }
                                if (flag_GameOver == 1)
                                {
                                    flag_conduct = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        //前方为绿箱子
                        if (mapMatrix[people_vertical_x][people_crosswise_y + 1] == 7)
                        {
                            if (mapMatrix[people_vertical_x][people_crosswise_y + 2] == 2) //检查更上面的单元
                            {
                                //此时是前方为绿箱子,再往前为道路
                                gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                                PrintMate(9); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + (people_crosswise_y + 2) * 2, offset_vertical + people_vertical_x);
                                PrintMate(4); //更改显示箱子位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_crosswise_y++;                                 //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵
                                mapMatrix[people_vertical_x][people_crosswise_y + 1] = 4;

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 34);

                                //控制播放推箱子离开目标点的音效
                                // mciSendString("play PushBoxing", NULL, 0, NULL);
                                PlaySound(TEXT("Music\\activation_change_fail.wav"), NULL, SND_FILENAME | SND_ASYNC);
                            }
                            else
                            {
                                if (mapMatrix[people_vertical_x][people_crosswise_y + 2] == 3) //检查更上面的单元
                                {
                                    //此时是前方为绿箱子,再往前为目标点
                                    gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                                    PrintMate(9); //更改显示人物位置
                                    gotoxy(offset_crosswise_prscise + (people_crosswise_y + 2) * 2, offset_vertical + people_vertical_x);
                                    PrintMate(7); //更改显示箱子位置
                                    gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                    PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                    mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                    people_crosswise_y++;                                 //更改人物位置信息
                                    mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵
                                    mapMatrix[people_vertical_x][people_crosswise_y + 1] = 7;

                                    //下面记录移动信息
                                    RecordMoveInfo(chain_head, 44);

                                    //控制播放推箱子上目标点的音效
                                    // mciSendString("play PushBoxing", NULL, 0, NULL);
                                    PlaySound(TEXT("Music\\add_favorite.wav"), NULL, SND_FILENAME | SND_ASYNC);
                                }
                            }
                        }
                        else
                        {
                            //前方为目标点
                            if (mapMatrix[people_vertical_x][people_crosswise_y + 1] == 3)
                            {
                                gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                                PrintMate(9); //更改显示人物位置
                                gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                                PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                                mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                                people_crosswise_y++;                                 //更改人物位置信息
                                mapMatrix[people_vertical_x][people_crosswise_y] = 9; //修正矩阵

                                //下面记录移动信息
                                RecordMoveInfo(chain_head, 54);
                            }
                        }
                    }
                }
                break;
            case 'R':
            case 'r':
                //用户想要重新开始
                return 1;
                break;
            case 'V':
            case 'v':
                PrintSaving(chain_head, level);
                //用户想要重新开始
                break;
            case 'm':
                //用户想返回菜单
                // 3.15开始维护这里
                //下面这个是设置缓冲区域的
                //请参考:https://blog.csdn.net/qq_43093230/article/details/103221955
                // SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), size);
                //需要清除画笔
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
                //修改字体大小
                cfi.dwFontSize.Y = 15;
                SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
                //设置黑窗口的界面大小
                system("mode con cols=90 lines=45"); // cols是列,一列占10像素,lines是行,一行占20像素
                //修改背景音乐的播放
                mciSendString("stop BackgroundMusic", NULL, 0, NULL);
                //然后循环播放背景音乐
                mciSendString("play MenuMusic repeat", NULL, 0, NULL);
                return 2;
                break;
            case 'U':
            case 'u':
                //接下来在这里写后退的方法
                //真不应该用二维数组
                cheack = chain_head->next;
                if (cheack->direction != 0)
                {
                    switch (cheack->direction)
                    {
                    case 1:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                        PrintMate(mapMatrix[people_vertical_x + 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                        people_vertical_x++;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵

                        break;
                    case 2:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                        PrintMate(mapMatrix[people_vertical_x - 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                        people_vertical_x--;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵

                        break;
                    case 3:
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y + 1] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                        people_crosswise_y++;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵

                        break;
                    case 4:
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y - 1] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                        people_crosswise_y--;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵

                        break;
                    case 11:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                        PrintMate(mapMatrix[people_vertical_x + 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(4); //更改显示箱子位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                        PrintMate(2); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 4;
                        people_vertical_x++;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x - 2][people_crosswise_y] = 2;
                        break;
                    case 12:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                        PrintMate(mapMatrix[people_vertical_x - 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(4); //更改显示箱子位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                        PrintMate(2); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 4;
                        people_vertical_x--;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x + 2][people_crosswise_y] = 2;
                        break;
                    case 13:
                        //推动箱子
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y + 1] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y)*2, offset_vertical + people_vertical_x);
                        PrintMate(4); //更改显示箱子位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(2); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 4;
                        people_crosswise_y++;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x][people_crosswise_y - 2] = 2;
                        break;
                    case 14:
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y - 1] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y)*2, offset_vertical + people_vertical_x);
                        PrintMate(4); //更改显示箱子位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(2); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 4;
                        people_crosswise_y--;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x][people_crosswise_y + 2] = 2;
                        break;
                    case 21:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                        PrintMate(mapMatrix[people_vertical_x + 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        if (mapMatrix[people_vertical_x][people_crosswise_y] == 8)
                        {
                            PrintMate(4); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = 4;
                        }
                        else
                        {
                            PrintMate(7); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        }
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                        PrintMate(3);                                                                                            //更改显示箱子位置
                        people_vertical_x++;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x - 2][people_crosswise_y] = 3;
                        break;
                    case 22:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                        PrintMate(mapMatrix[people_vertical_x - 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        if (mapMatrix[people_vertical_x][people_crosswise_y] == 8)
                        {
                            PrintMate(4); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = 4;
                        }
                        else
                        {
                            PrintMate(7); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        }
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                        PrintMate(3);                                                                                            //更改地皮显示
                        people_vertical_x--;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x + 2][people_crosswise_y] = 3;
                        break;
                    case 23:
                        //推动箱子
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y + 1] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y)*2, offset_vertical + people_vertical_x);
                        if (mapMatrix[people_vertical_x][people_crosswise_y] == 8)
                        {
                            PrintMate(4); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = 4;
                        }
                        else
                        {
                            PrintMate(7); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        }
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(3);                                                                                            //更改地皮显示
                        people_crosswise_y++;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x][people_crosswise_y - 2] = 3;
                        break;
                    case 24:
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y - 1] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y)*2, offset_vertical + people_vertical_x);
                        if (mapMatrix[people_vertical_x][people_crosswise_y] == 8)
                        {
                            PrintMate(4); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = 4;
                        }
                        else
                        {
                            PrintMate(7); //更改地皮显示
                            mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        }
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(3);                                                                                            //更改地皮显示
                        people_crosswise_y--;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x][people_crosswise_y + 2] = 3;
                        break;
                    case 31:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                        PrintMate(mapMatrix[people_vertical_x + 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(7); //更改显示箱子位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                        PrintMate(2); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        people_vertical_x++;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x - 2][people_crosswise_y] = 2;
                        break;
                    case 32:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                        PrintMate(mapMatrix[people_vertical_x - 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(7);
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                        PrintMate(2); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        people_vertical_x--;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x + 2][people_crosswise_y] = 2;
                        break;
                    case 33:
                        //此时是前方为绿箱子,再往前为道路
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y + 1] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y)*2, offset_vertical + people_vertical_x);
                        PrintMate(7); //更改显示箱子位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(2); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        people_crosswise_y++;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x][people_crosswise_y - 2] = 2;
                        break;
                    case 34:
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y - 1] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y)*2, offset_vertical + people_vertical_x);
                        PrintMate(7); //更改显示箱子位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(2); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        people_crosswise_y--;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x][people_crosswise_y + 2] = 2;
                        break;
                    case 41:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                        PrintMate(mapMatrix[people_vertical_x + 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(7); //更改显示箱子位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                        PrintMate(3); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        people_vertical_x++;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x - 2][people_crosswise_y] = 3;
                        break;
                    case 42:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                        PrintMate(mapMatrix[people_vertical_x - 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(7);
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                        PrintMate(3); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        people_vertical_x--;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x + 2][people_crosswise_y] = 3;
                        break;
                    case 43:
                        //此时是前方为绿箱子,再往前为道路
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x + 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y)*2, offset_vertical + people_vertical_x);
                        PrintMate(7); //更改显示箱子位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(3); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        people_crosswise_y++;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x][people_crosswise_y - 2] = 3;
                        break;
                    case 44:
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x + 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y)*2, offset_vertical + people_vertical_x);
                        PrintMate(7); //更改显示箱子位置
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(3); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = 7;
                        people_crosswise_y--;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵
                        mapMatrix[people_vertical_x][people_crosswise_y + 2] = 3;
                        break;
                    case 51:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x + 1);
                        PrintMate(mapMatrix[people_vertical_x + 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                        people_vertical_x++;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵

                        break;
                    case 52:
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x - 1);
                        PrintMate(mapMatrix[people_vertical_x - 1][people_crosswise_y] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                        people_vertical_x--;                                                                                     //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵

                        break;
                    case 53:
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y + 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y + 1] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                        people_crosswise_y++;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵

                        break;
                    case 54:
                        gotoxy(offset_crosswise_prscise + (people_crosswise_y - 1) * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y - 1] + 6); //更改显示人物位置
                        gotoxy(offset_crosswise_prscise + people_crosswise_y * 2, offset_vertical + people_vertical_x);
                        PrintMate(mapMatrix[people_vertical_x][people_crosswise_y] - 6); //更改地皮显示
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] - 6;
                        people_crosswise_y--;                                                                                    //更改人物位置信息
                        mapMatrix[people_vertical_x][people_crosswise_y] = mapMatrix[people_vertical_x][people_crosswise_y] + 6; //修正矩阵

                        break;
                    default:
                        printf("ERROR!_S2");
                        break;
                    }
                    //删除节点
                    chain_head->next = cheack->next;
                    free(cheack);

                    //减少步数
                    chain_head->direction--;
                    PrintStepNumber(chain_head->direction);

                    //播放倒退的音效
                    // mciSendString("play PushBoxing", NULL, 0, NULL);
                    PlaySound(TEXT("Music\\BREEZE.wav"), NULL, SND_FILENAME | SND_ASYNC);
                }
                break;
            default:
                break;
            }
            if (flag_conduct == 0)
            {
                //保存进度为下一关
                SaveLevelToFile(level + 1);

                //先欢腾1秒，再切换界面
                //控制走在路上的音效
                PlaySound(TEXT("Music\\APPLAUSE.wav"), NULL, SND_FILENAME | SND_ASYNC);
                Sleep(1000);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY); //清理画笔颜色
                //清屏
                system("cls");

                //控制留多少空行
                for (int i = 0; i < 4; i++)
                {
                    printf("\n");
                }
                printf("\t=======================================\n");
                printf("\t=                                     =\n");
                printf("\t=            Congratulation!!         =\n");
                printf("\t=                                     =\n");
                printf("\t=              [N]ext level           =\n");
                printf("\t=              [R]epeated             =\n");
                printf("\t=              [M]enu                 =\n");
                printf("\t=                                     =\n");
                printf("\t=======================================\n");
                while (flag_conduct != 2)
                {
                    switch (getch())
                    {
                    case 'N':
                    case 'n':
                        //进行下一关前先把目标文件清除干净
                        filename_forClean[20] = (level + 1) / 10 + '0'; //轻易不要改变传入函数的参数
                        filename_forClean[21] = (level + 1) % 10 + '0';
                        if ((file_forClean = fopen(filename_forClean, "w")) == NULL)
                        {
                            // printf("%s", filename_forClean);
                            // printf("ERROR:fail to open the file to clean");
                            //如果没有找到文件，就不要清理
                        }
                        else
                        {
                            //开始写入文件，进行清理
                            fprintf(file_forClean, 0);
                        }
                        fclose(file_forClean); //血的教训
                        return 0;              // 0代表进行下一关
                        break;
                    case 'M':
                    case 'm':
                        //用户想返回菜单
                        // 3.15开始维护这里
                        //下面这个是设置缓冲区域的
                        //请参考:https://blog.csdn.net/qq_43093230/article/details/103221955
                        // SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), size);
                        //需要清除画笔
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
                        //修改字体大小
                        cfi.dwFontSize.Y = 15;
                        SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
                        //设置黑窗口的界面大小
                        system("mode con cols=90 lines=45"); // cols是列,一列占10像素,lines是行,一行占20像素
                        //修改背景音乐的播放
                        mciSendString("stop BackgroundMusic", NULL, 0, NULL);
                        //然后循环播放背景音乐
                        mciSendString("play MenuMusic repeat", NULL, 0, NULL);
                        return 2;
                        break;
                    case 'R':
                    case 'r':
                        //先保存文件
                        SaveData(chain_head, level);
                        //返回一个特殊的值,然后读取刚刚保存的数据
                        return 3;
                        break;
                    }
                }
            }
        }

        //此时退出了while循环读取
        //若返回值为0则表示正确通关
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY); //函数结束前应该洗刷画笔

        return flag_conduct;
    }
}

//打印单元格
void PrintMate(int kind)
{
    switch (kind)
    {
    case 0:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 171);

        printf("  "); //绿色草皮
        break;
    case 1:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 75);

        printf("■"); //红蓝墙体
        break;
    case 2:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 119);

        printf("  "); //灰色地板
        break;
    case 3:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 114);

        printf("○"); //绿圈目标地点
        break;
    case 4:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 126); //箱子的颜色

        printf("■"); //蛋黄色箱子
        break;
    case 8:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 121); //红色

        printf("★"); //蓝色星星
        break;
    case 7:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 122); //绿色目标位置  方块

        printf("■"); //蓝色星星
        break;
    case 9:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 124);

        printf("☆"); //红色边框星星
        break;
    default:
        printf("ERROR in Data");
        break;
    }
}

//记录移动方向函数
void RecordMoveInfo(Info_move *chainHead, int direction)
{
    //采用头插法
    Info_move *new = (Info_move *)malloc(sizeof(Info_move));
    new->next = chainHead->next;
    new->direction = direction;
    chainHead->next = new;
    chainHead->direction++; //步数加一
    PrintStepNumber(chainHead->direction);
}

//绘制提示信息
void PrintInfo(int level)
{
    //清除画笔
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
    //预留大小60横向x,20竖向y
    gotoxy(40, 2);
    printf("Level:%d", level);
    gotoxy(40, 3);
    printf("W,w,↑:up");
    gotoxy(40, 4);
    printf("S,s,↓:down");
    gotoxy(40, 5);
    printf("A,a,←:left");
    gotoxy(40, 6);
    printf("D,d,→:right");
    gotoxy(40, 7);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 11);
    printf("U");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
    printf("ndo");
    gotoxy(40, 8);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 11);
    printf("R");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
    printf("estart");
    gotoxy(40, 9);
    printf("Moves:");
    gotoxy(40, 10);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 11);
    printf("m");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
    printf("enu");
    gotoxy(40, 11);
    printf("Sa");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 11);
    printf("v");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
    printf("e");
}

//改写步数
void PrintStepNumber(int stepNumber)
{
    //清除画笔
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
    //预留大小60横向x,20竖向y
    gotoxy(40, 9);
    printf("Moves:%d     ", stepNumber);
}

//保存行动数据
void SaveData(Info_move *head, int level)
{
    int moves;
    FILE *outputInf;
    int inf;
    char filename[30] = {"information\\ActionOf00.txt"};
    filename[20] = level / 10 + '0';
    filename[21] = level % 10 + '0';
    Info_move *output_number;
    if ((outputInf = fopen(filename, "w")) == NULL)
    {
        printf("%s", filename);
        printf("ERROR:fail to open the output file");
    }
    if (head == NULL)
    {
        fprintf(outputInf, "%d", 0);
        fclose(outputInf);
        return;
    }
    output_number = head->next;
    moves = head->direction;
    fprintf(outputInf, "%d ", moves);
    while (moves--)
    {
        inf = output_number->direction;
        fprintf(outputInf, "%d ", inf);
        output_number = output_number->next;
    }

    fclose(outputInf);

    //测试用
    // printf("success");
    //这里要写一个闪烁的success
}

//加载进度
Info_move *LoadingSchedule(int level)
{
    int moves;
    char filename[30] = {"information\\ActionOf00.txt"};
    filename[20] = level / 10 + '0';
    filename[21] = level % 10 + '0';
    Info_move *input_number;
    FILE *inputInf;
    if ((inputInf = fopen(filename, "r")) == NULL)
    {
        printf("%s", filename);
        printf("ERROR:fail to open the input file");
    }
    //首先读入步数--->必读
    fscanf(inputInf, "%d", &moves);
    if (moves == 0)
    {
        fclose(inputInf); //血的教训,return前释放文件啊喂
        return NULL;
    }
    Info_move *chain_head = (Info_move *)malloc(sizeof(Info_move));
    Info_move *chain_rear = (Info_move *)malloc(sizeof(Info_move));
    chain_rear->direction = 0;
    chain_rear->next = NULL;
    chain_head->direction = moves;
    chain_head->next = chain_rear;
    Info_move *new, *rear = chain_head;
    //若步数为0则直接退出
    while (moves--)
    {
        new = (Info_move *)malloc(sizeof(Info_move));
        fscanf(inputInf, "%d", &(new->direction));
        //使用头插法
        new->next = chain_head->next;
        chain_head->next = new;
    }
    // new = (Info_move *)malloc(sizeof(Info_move));
    // new->direction = 0;
    // new->next = NULL;
    // rear->next = new;
    fclose(inputInf);
    return chain_head;
}
//打印保存中
void PrintSaving(Info_move *head, int level)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
    gotoxy(40, 12);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 206);
    printf("Saving:70%%");
    SaveData(head, level);
    Sleep(500);
    gotoxy(40, 12);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | 234);
    printf("Saving:100%%");
    Sleep(300);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
    gotoxy(40, 12);
    printf("                ");
}

//保存关卡进度
void SaveLevelToFile(int level)
{
    FILE *output;
    if ((output = fopen("information\\mainInf.txt", "w")) == NULL)
    {
        printf("ERROR to open file for write level at mainInf.txt");
    }
    fprintf(output, "%d", level);
    fclose(output);
}