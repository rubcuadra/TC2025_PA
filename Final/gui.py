import pygame
from onitampy.board import OnitamaBoard
from onitampy.movements import OnitamaCards
from codes import *
import os

#DO NOT TOUCH
BOARD_SIZE = 5     # 5x5
SCREEN = None
CLOCK  = None

BLACK    = (  0,   0,   0)
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
CREAM    = (223, 216, 201) 
DARKGRAY = (101,  97,  87)
CUSTOM   = (197, 189, 186)

HEADER_PNG       = pygame.image.load(os.path.join("assets/header.png"))
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
CARD_WIDTH  = 150
CARD_HEIGHT = 73 
WINDOWWIDTH = 640  # size of window's width in pixels
WINDOWHEIGHT= 480  # size of windows' height in pixels
FPS = 60           # frames per second, the general speed of the program
BOXSIZE = 40       # size of box height & width in pixels
GAPSIZE = 10       # size of gap between boxes in pixels
BOARDWIDTH = 10    # number of columns of icons
BOARDHEIGHT = 7    # number of rows of icons
XMARGIN = int((WINDOWWIDTH - (BOARDWIDTH * (BOXSIZE + GAPSIZE))) / 2)
YMARGIN = int((WINDOWHEIGHT - (BOARDHEIGHT * (BOXSIZE + GAPSIZE))) / 2) + 40

BGCOLOR          = CREAM
BOXCOLOR         = WHITE
SELECTEDBOXCOLOR = YELLOW
HIGHLIGHTCOLOR   = YELLOW

def getBoxAtPixel(x, y):
    for boxx in range(BOARDWIDTH):
        for boxy in range(BOARDHEIGHT):
            left, top = leftTopCoordsOfBox(boxx, boxy)
            boxRect = pygame.Rect(left, top, BOXSIZE, BOXSIZE)
            if boxRect.collidepoint(x, y):
                return (boxx, boxy)
    return (None, None)

def leftTopCoordsOfBox(boxx, boxy):
    # Convert board coordinates to pixel coordinates
    left = boxx * (BOXSIZE + GAPSIZE) + XMARGIN
    top = boxy * (BOXSIZE + GAPSIZE) + YMARGIN
    return (left, top)

def getBoxAtPixel(x, y):
    for boxx in range(BOARD_SIZE):
        for boxy in range(BOARD_SIZE):
            left, top = leftTopCoordsOfBox(boxx, boxy)
            boxRect = pygame.Rect(left, top, BOXSIZE, BOXSIZE)
            if boxRect.collidepoint(x, y):
                return (boxx, boxy)
    return (None, None)

def getClickedCard(x,y): 
    left, top = leftTopCoordsOfBox(BOARD_SIZE, 3)
    #Index 0
    boxRect = pygame.Rect( GAPSIZE+left,top+(BOXSIZE/2), CARD_WIDTH, CARD_HEIGHT)
    if boxRect.collidepoint(x, y): return 0
    #Index 1
    boxRect = pygame.Rect( GAPSIZE+left+CARD_WIDTH,top+(BOXSIZE/2), CARD_WIDTH, CARD_HEIGHT)
    if boxRect.collidepoint(x, y): return 1
    #Did not click
    return None

class Onitama_GUI():
    def __init__(self, board, player=OnitamaBoard.BLUE,turn=OnitamaBoard.BLUE):
        self.board         = board
        self.player        = player
        self.turn          = turn
        self.is_running    = False
        self.winner        = None
        self.resetMovement()

    def drawBoard(self):
        for boxx in range(BOARD_SIZE): # Draws cells
            for boxy in range(BOARD_SIZE):
                left, top = leftTopCoordsOfBox(boxx, boxy)
                if self.selected_cell[0] == boxx and self.selected_cell[1] == boxy:
                    pygame.draw.rect(self.SCREEN, SELECTEDBOXCOLOR, (left, top, BOXSIZE, BOXSIZE))
                else: #Draw normal Cell
                    pygame.draw.rect(self.SCREEN, BOXCOLOR, (left, top, BOXSIZE, BOXSIZE))
                
                token = self.board[boxy][boxx] if self.player == self.board.BLUE else self.board[BOARD_SIZE-boxy-1][BOARD_SIZE-boxx-1] 
                if   token == self.board.BLUE_MASTER:  self.SCREEN.blit(BLUE_MASTER_PNG,  (left,top) )
                elif token == self.board.RED_MASTER:   self.SCREEN.blit(RED_MASTER_PNG,   (left,top) )
                elif token == self.board.BLUE_STUDENT: self.SCREEN.blit(BLUE_STUDENT_PNG, (left,top) )
                elif token == self.board.RED_STUDENT:  self.SCREEN.blit(RED_STUDENT_PNG,  (left,top) )    

    def drawCards(self):
        if self.player == self.board.BLUE:
            opponentCards = self.board.cards[1] 
            ourCards      = self.board.cards[0] 
        else:
            opponentCards = self.board.cards[0]
            ourCards      = self.board.cards[1]
        
        #OPPONENT
        left, top = leftTopCoordsOfBox(BOARD_SIZE, 0)
        for i,card in enumerate(opponentCards): 
            opponent_card = pygame.transform.rotate(CARDS_IMG[card], 180) #Rotate upside down
            self.SCREEN.blit( opponent_card, (GAPSIZE+left+CARD_WIDTH*i,top) )
        #WE
        left, top = leftTopCoordsOfBox(BOARD_SIZE, 3)
        for i,card in enumerate(ourCards): 
            x = GAPSIZE+left+CARD_WIDTH*i
            y = top+(BOXSIZE/2)
            if self.selected_card == card: pygame.draw.rect(self.SCREEN, HIGHLIGHTCOLOR, (x - 5, y - 5, CARD_WIDTH-15 , CARD_HEIGHT + 10), 4)
            self.SCREEN.blit(  CARDS_IMG[card],  (x,y) )
            self.OUR_CARDS[i] = card
        
        #STAND_BY
        left, top = leftTopCoordsOfBox(BOARD_SIZE, 2)
        if self.turn == self.player: #Our turn, print normally
            self.SCREEN.blit(  CARDS_IMG[self.board.cards[2]],  (GAPSIZE+left+(CARD_WIDTH/2),top-(BOXSIZE/4)-5) )
        else:              #Opponent, upsidedown
            opponent_card = pygame.transform.rotate(CARDS_IMG[self.board.cards[2]], 180) #Rotate upside down
            self.SCREEN.blit(  opponent_card,  (GAPSIZE+left+(CARD_WIDTH/2),top-(BOXSIZE/4)-5) )

    def drawMovements(self):
        self.valid_movs.clear()
        if self.selected_card!=None and self.selected_cell[0]!=None and self.selected_cell[1]!=None:
            for movement in OnitamaCards[self.selected_card]:
                x,y = self.selected_cell[0]+movement[1],self.selected_cell[1]+movement[0]
                if (x>=0 and x<BOARD_SIZE) and (y>=0 and y<BOARD_SIZE): #Movement in range
                    token = self.board[y][x] if self.player == self.board.BLUE else self.board[BOARD_SIZE-1-y][BOARD_SIZE-1-x]
                    if not self.board.isPlayer(self.player,token): #VALID MOVEMENT
                        left, top = leftTopCoordsOfBox(x, y)
                        pygame.draw.rect(self.SCREEN, SELECTEDBOXCOLOR, (left, top, BOXSIZE, BOXSIZE))
                        self.valid_movs.add( (x,y) )#SAVE FOR LATER
    def getSelectedMovement(self):
        if self.from_cell[0]!=None and self.from_cell[1]!=None and self.selected_card!=None and self.to_cell[0]!=None and self.to_cell[1]!=None:
            return (self.from_cell,self.selected_card,self.to_cell)
        return None
    
    def resetMovement(self):
        self.OUR_CARDS     = [None,None]
        self.selected_cell = (None,None) #TOKEN
        self.selected_card = None        #CARD
        self.valid_movs    = set([])
        self.from_cell     = (None,None)
        self.to_cell       = (None,None) #Important for client
    
    def drawText(self):
        #HEADER
        self.SCREEN.blit(  HEADER_PNG,  (0,0) )
        #TURNS
        left,top = leftTopCoordsOfBox(0,BOARD_SIZE)
        text = None
        if self.winner != None: #GameOver
            text = self.winnerIsBlueText if self.winner == self.board.BLUE else self.winnerIsRedText
        else:
            if self.player == self.turn: text = self.ourTurnText
            else:                        text = self.opponentTurnText
        self.SCREEN.blit(text,(left,top+GAPSIZE))

    def loadText(self):
        pygame.font.init()
        self.font   = pygame.font.SysFont('Comic Sans MS', 30)
        self.ourTurnText      = self.font.render('Your Turn', False, BLACK)
        self.opponentTurnText = self.font.render("Waiting for opponent to move ", False, BLACK)
        self.winnerIsBlueText = self.font.render("GAME OVER, THE WINNER IS BLUE ", False, BLACK)
        self.winnerIsRedText  = self.font.render("GAME OVER, THE WINNER IS RED ", False, BLACK)
    
    def prepareSprites(self):
        BLUE_MASTER_PNG.convert()
        RED_MASTER_PNG.convert()
        BLUE_STUDENT_PNG.convert()
        RED_STUDENT_PNG.convert()
        HEADER_PNG.convert()
        for c in self.board.cards[0]: CARDS_IMG[c].convert()
        for c in self.board.cards[1]: CARDS_IMG[c].convert()
        CARDS_IMG[self.board.cards[2]].convert()

    def run(self):
        pygame.init()
        self.SCREEN = pygame.display.set_mode((WINDOWWIDTH, WINDOWHEIGHT))
        self.CLOCK  = pygame.time.Clock()
        self.is_running = True
        self.done = False
        x = 30
        y = 30
        mousex,mousey = 0,0 # used to store coordinates of mouse event
        pygame.display.set_caption('Onitama')
        self.SCREEN.fill(BGCOLOR)

        #Onitama Sprites
        self.prepareSprites()
        #TEXT
        self.loadText()

        while not self.done:
            mouseClicked = False
            self.SCREEN.fill(BGCOLOR) # drawing the window
            
            self.drawBoard()
            self.drawCards()
            self.drawMovements()
            self.drawText()

            for event in pygame.event.get(): # event handling loop
                    if event.type == pygame.QUIT or (event.type == pygame.KEYUP and event.key == pygame.K_ESCAPE):
                            self.done = True
                    elif event.type == pygame.MOUSEMOTION:
                            mousex, mousey = event.pos
                    elif event.type == pygame.MOUSEBUTTONUP:
                            mousex, mousey = event.pos
                            mouseClicked = True
            
            #We only interact if it is our turn and game is not over
            if self.turn == self.player and self.winner==None:
                #CHECK CLICK ON CELLS
                boxx, boxy = getBoxAtPixel(mousex, mousey)
                if (boxx != None and boxy != None) and mouseClicked: # The mouse is currently over a box and recieved a click
                    token = self.board[boxy][boxx] if self.player == self.board.BLUE else self.board[BOARD_SIZE-boxy-1][BOARD_SIZE-boxx-1] 
                    if self.board.isPlayer(self.player,token): #CHECK IT IS OUR TOKEN
                        self.selected_cell = (boxx,boxy)
                    elif (boxx,boxy) in self.valid_movs: #CHECK CLICK ON MOVEMENT CHOICE - DESTINATION CELL
                        self.from_cell = (self.selected_cell[1],self.selected_cell[0]) if self.player == self.board.BLUE else (BOARD_SIZE-1-self.selected_cell[1],BOARD_SIZE-1-self.selected_cell[0])
                        self.to_cell   = (boxy,boxx)   if self.player == self.board.BLUE else (BOARD_SIZE-1-boxy,BOARD_SIZE-1-boxx)
                        print(self.getSelectedMovement())
                #CHECK CLICK ON CARDS
                card = getClickedCard(mousex, mousey)
                if card!=None and mouseClicked:
                    self.selected_card = self.OUR_CARDS[card]
            
            pygame.display.flip()
            # will block execution until 1/60 seconds have passed 
            # since the previous time clock.tick was called.
            self.CLOCK.tick(FPS)

if __name__ == '__main__':
        board = OnitamaBoard()
        gui = Onitama_GUI(board,player=OnitamaBoard.BLUE,turn=OnitamaBoard.BLUE)
        gui.run()
        