// Ruben Cuadra A01019102
typedef enum valid_options {PVE, PVP, EXIT} ops_t;
typedef enum valid_responses {OK, BYE, ERROR=24, WRONG_TABLE=25, WRONG_DIFFICULTY=26,WRONG_MOVEMENT=27,WRONG_CARD=28, GAME_STARTED} response_t;
typedef enum valid_tables {RANDOM_TABLE = -1} vt; 
//EMPTY => No Players       
//FREE  => 1 player missing 
typedef enum table_status {EMPTY,FREE,PLAYING} table_s;
