#RUBEN CUADRA A01019102
from onitampy.board import OnitamaBoard
from onitampy.agent import OnitamaPlayer

# RUN THE EXAMPLE AND IT will tell you the best movement in that case
if __name__ == '__main__':
    from sys import argv
    #Example: 
    #   Params -                       "BOARD;cards" PLAYER LEVEL
    #   python Player/agent.py "4434400000000000000022122;HORSE TIGER;DRAGON MANTIS;ROOSTER" 0 2
    # argv[0] => thisFile.py
    board  = OnitamaBoard.fromArgs(argv[1]) 
    player = OnitamaBoard.BLUE if argv[2]=='0' else OnitamaBoard.RED 
    level  = int(argv[3]) #1-3
    op = OnitamaPlayer(player,level)
    movement = op.getBestMovement(board)
    #fromRow,fromCol;MOVEMENT;toRow,toCol
    print( "%s %s %s %s %s"%(movement[0][0],movement[0][1],movement[2][0],movement[2][1],movement[1])  )

    # board = OnitamaBoard()
    # board.cards[1].pop()
    # board.cards[1].add('ELEPHANT')
    # print(board.canMove( board.RED, (0,2),"ELEPHANT",(1,3)))
    # ((0, 2), , (1, 3))
    # 
    #python game_logic.py "4034400400000000002022102;EEL COBRA;MANTIS CRANE;MONKEY" 0 2