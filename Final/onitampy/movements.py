from .randomDict import RandomDict as RD

#Row, Column
DR   = ( 1, 1)
DL   = ( 1,-1)
UR   = (-1, 1)
UL   = (-1,-1)
U    = (-1, 0)
UU   = (-2, 0)
D    = ( 1, 0)
R    = ( 0, 1)
RR   = ( 0, 2)
L    = ( 0,-1)
LL   = ( 0,-2)
LLU  = (-1,-2)
RRU  = (-1, 2)

#Para BLUE (Player que va de abajo hacia arriba)
OnitamaCards = RD({
    #Adjacent
    "MANTIS"   : ( UL, UR, D ),
    "OX"       : ( U , R,  D ),
    "HORSE"    : ( U , L,  D ),
    "EEL"      : ( UL, DL, R ),
    "CRANE"    : ( DL, DR, U ),
    "BOAR"     : ( L , R,  U ),
    "COBRA"    : ( L , UR, DR),
    "MONKEY"   : ( UL, UR, DL, DR),
    "ELEPHANT" : ( UL, UR, L , R),
    "ROOSTER"  : ( DL, UR, L , R),
    "GOOSE"    : ( UL, DR, L , R),
    #Jumps
    "TIGER"    : ( UU, D     ),
    "RABBIT"   : ( DL, UR, RR),
    "FROG"     : ( DR, UL, LL),
    "CRAB"     : ( LL, U , RR) ,
    "DRAGON"   : ( DL, DR, LLU, RRU),
})

DECK_INDEXES = [
    "MANTIS",
    "OX",
    "HORSE",
    "EEL",
    "CRANE",
    "BOAR",
    "COBRA",
    "MONKEY",
    "ELEPHANT",
    "ROOSTER",
    "GOOSE",
    "TIGER",
    "RABBIT",
    "FROG",
    "CRAB",
    "DRAGON",
]

MOVEMENT_ID = {
    #Adjacent
    "MANTIS"   : 0,
    "OX"       : 1,
    "HORSE"    : 2,
    "EEL"      : 3,
    "CRANE"    : 4,
    "BOAR"     : 5,
    "COBRA"    : 6,
    "MONKEY"   : 7,
    "ELEPHANT" : 8,
    "ROOSTER"  : 9,
    "GOOSE"    : 10,
    #Jumps
    "TIGER"    : 11,
    "RABBIT"   : 12,
    "FROG"     : 13,
    "CRAB"     : 14,
    "DRAGON"   : 15,
}