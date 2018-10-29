typedef enum valid_options {PVE, PVP, EXIT} ops_t;
typedef enum valid_responses {OK, BYE, ERROR, WRONG_TABLE, WRONG_DIFFICULTY,WRONG_MOVEMENT,WRONG_CARD, MOVEMENT_DONE, GAME_STARTED} response_t;
typedef enum valid_tables {RANDOM_TABLE = -1} vt; 
//EMPTY => No Players       
//FREE  => 1 player missing 
typedef enum table_status {EMPTY,FREE,PLAYING} table_s;
