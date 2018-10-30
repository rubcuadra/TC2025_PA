import pygame
from onitampy.board import OnitamaBoard
import os
#DO NOT TOUCH
BOARD_SIZE = 5     # 5x5
SCREEN = None
CLOCK  = None

GRAY     = (100, 100, 100)
NAVYBLUE = ( 60,  60, 100)
WHITE    = (255, 255, 255)
RED      = (255,   0,   0)
GREEN    = (  0, 255,   0)
BLUE     = (  0,   0, 255)
YELLOW   = (255, 255,   0)
ORANGE   = (255, 128,   0)
PURPLE   = (255,   0, 255)
CYAN     = (  0, 255, 255)

BLUE_MASTER_PNG  = pygame.image.load(os.path.join("assets/blue_master.png"))
RED_MASTER_PNG   = pygame.image.load(os.path.join("assets/red_master.png"))
BLUE_STUDENT_PNG = pygame.image.load(os.path.join("assets/blue_student.png"))
RED_STUDENT_PNG  = pygame.image.load(os.path.join("assets/red_student.png"))
CARDS_IMG = {
    "BOAR"    : pygame.image.load(os.path.join("assets/cards/BOAR.jpeg")),
    "COBRA"   : pygame.image.load(os.path.join("assets/cards/COBRA.jpeg")),
    "CRAB"    : pygame.image.load(os.path.join("assets/cards/CRAB.jpeg")),
    "CRANE"   : pygame.image.load(os.path.join("assets/cards/CRANE.jpg")),
    "DRAGON"  : pygame.image.load(os.path.join("assets/cards/DRAGON.jpeg")),
    "EEL"     : pygame.image.load(os.path.join("assets/cards/EEL.jpeg")),
    "ELEPHANT": pygame.image.load(os.path.join("assets/cards/ELEPHANT.jpeg")),
    "FROG"    : pygame.image.load(os.path.join("assets/cards/FROG.jpeg")),
    "GOOSE"   : pygame.image.load(os.path.join("assets/cards/GOOSE.jpeg")),
    "HORSE"   : pygame.image.load(os.path.join("assets/cards/HORSE.jpeg")),
    "MANTIS"  : pygame.image.load(os.path.join("assets/cards/MANTIS.jpeg")),
    "MONKEY"  : pygame.image.load(os.path.join("assets/cards/MONKEY.jpeg")),
    "OX"      : pygame.image.load(os.path.join("assets/cards/OX.jpeg")),
    "RABBIT"  : pygame.image.load(os.path.join("assets/cards/RABBIT.jpg")),
    "ROOSTER" : pygame.image.load(os.path.join("assets/cards/ROOSTER.jpeg")),
    "TIGER"   : pygame.image.load(os.path.join("assets/cards/TIGER.jpg")),
}

WINDOWWIDTH = 640  # size of window's width in pixels
WINDOWHEIGHT = 480 # size of windows' height in pixels
FPS = 60           # frames per second, the general speed of the program
BOXSIZE = 40       # size of box height & width in pixels
GAPSIZE = 10       # size of gap between boxes in pixels
BOARDWIDTH = 10    # number of columns of icons
BOARDHEIGHT = 7    # number of rows of icons
XMARGIN = int((WINDOWWIDTH - (BOARDWIDTH * (BOXSIZE + GAPSIZE))) / 2)
YMARGIN = int((WINDOWHEIGHT - (BOARDHEIGHT * (BOXSIZE + GAPSIZE))) / 2)

BGCOLOR = NAVYBLUE
LIGHTBGCOLOR = GRAY
BOXCOLOR = WHITE
SELECTEDBOXCOLOR = GREEN
HIGHLIGHTCOLOR = BLUE

def getBoxAtPixel(x, y):
    for boxx in range(BOARDWIDTH):
        for boxy in range(BOARDHEIGHT):
            left, top = leftTopCoordsOfBox(boxx, boxy)
            boxRect = pygame.Rect(left, top, BOXSIZE, BOXSIZE)
            if boxRect.collidepoint(x, y):
                return (boxx, boxy)
    return (None, None)

def main():
    global SCREEN,CLOCK
    pygame.init()
    SCREEN = pygame.display.set_mode((WINDOWWIDTH, WINDOWHEIGHT))
    CLOCK  = pygame.time.Clock()

    done = False
    x = 30
    y = 30
    mousex,mousey = 0,0 # used to store coordinates of mouse event
    pygame.display.set_caption('Onitama')
    firstSelection = None
    SCREEN.fill(BGCOLOR)

    #Onitama Logic
    selected = (None,None)
    board = OnitamaBoard()
    player = OnitamaBoard.RED

    #Onitama Sprites
    BLUE_MASTER_PNG.convert()
    RED_MASTER_PNG.convert()
    BLUE_STUDENT_PNG.convert()
    RED_STUDENT_PNG.convert()
    for c in board.cards[0]: CARDS_IMG[c].convert()
    for c in board.cards[1]: CARDS_IMG[c].convert()
    CARDS_IMG[board.cards[2]].convert()
    
    while not done:
        mouseClicked = False
        SCREEN.fill(BGCOLOR) # drawing the window
        
        drawBoard(player,board,selected)

        for event in pygame.event.get(): # event handling loop
                if event.type == pygame.QUIT or (event.type == pygame.KEYUP and event.key == pygame.K_ESCAPE):
                        done = True
                elif event.type == pygame.MOUSEMOTION:
                        mousex, mousey = event.pos
                elif event.type == pygame.MOUSEBUTTONUP:
                        mousex, mousey = event.pos
                        mouseClicked = True
        
        boxx, boxy = getBoxAtPixel(mousex, mousey)
        if boxx != None and boxy != None: # The mouse is currently over a box.
            if mouseClicked: 
                token = board[boxy][boxx] if player == board.BLUE else board[BOARD_SIZE-boxy-1][boxx] 
                if board.isPlayer(player,token): #CHECK IT IS OUR TOKEN
                    selected = (boxx,boxy)

        pygame.display.flip()
        # will block execution until 1/60 seconds have passed 
        # since the previous time clock.tick was called.
        CLOCK.tick(FPS)

def leftTopCoordsOfBox(boxx, boxy):
    # Convert board coordinates to pixel coordinates
    left = boxx * (BOXSIZE + GAPSIZE) + XMARGIN
    top = boxy * (BOXSIZE + GAPSIZE) + YMARGIN
    return (left, top)

def drawBoard(player,board,selected):
    # Draws cells
    for boxx in range(BOARD_SIZE):
        for boxy in range(BOARD_SIZE):
            left, top = leftTopCoordsOfBox(boxx, boxy)
            if selected[0] == boxx and selected[1] == boxy:
                pygame.draw.rect(SCREEN, SELECTEDBOXCOLOR, (left, top, BOXSIZE, BOXSIZE))
            else: #Draw normal Cell
                pygame.draw.rect(SCREEN, BOXCOLOR, (left, top, BOXSIZE, BOXSIZE))
            
            token = board[boxy][boxx] if player == board.BLUE else board[BOARD_SIZE-boxy-1][boxx] 
            if   token == board.BLUE_MASTER:  SCREEN.blit(BLUE_MASTER_PNG,  (left,top) )
            elif token == board.RED_MASTER:   SCREEN.blit(RED_MASTER_PNG,   (left,top) )
            elif token == board.BLUE_STUDENT: SCREEN.blit(BLUE_STUDENT_PNG, (left,top) )
            elif token == board.RED_STUDENT:  SCREEN.blit(RED_STUDENT_PNG,  (left,top) )
    
    # if player == board.BLUE: #PRINT BLUE FOR US
    # player_image = pg.transform.rotate(xwingImg, angle)
    # player_rect = player_image.get_rect()
    # player_rect.center = player_pos
    # screen.blit(player_image, player_rect)
def getBoxAtPixel(x, y):
    for boxx in range(BOARD_SIZE):
        for boxy in range(BOARD_SIZE):
            left, top = leftTopCoordsOfBox(boxx, boxy)
            boxRect = pygame.Rect(left, top, BOXSIZE, BOXSIZE)
            if boxRect.collidepoint(x, y):
                return (boxx, boxy)
    return (None, None)

if __name__ == '__main__':
        main()