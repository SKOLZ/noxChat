#define TRUE 1;
#define FALSE -1;
typedef int boolean;
#define MESSAGE_SIZE 140
#define NAME_SIZE 32
#define CHAT_ROWS 20
#define CHAT_COLS 80
#define MAX_ROOM_DIGITS 2
#define BREAKLINE printf("\n")

void saveData(void);
void chatRoom(int serverNumber, int processID);
char* itoa(int n, char s[]);
void reverse(char s[]);
void showRooms(void);
void shutdown(int status);