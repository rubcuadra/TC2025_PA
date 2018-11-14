from onitampy.board import OnitamaBoard
from onitampy.movements import OnitamaCards,DECK_INDEXES

if __name__ == '__main__':
    board = OnitamaBoard()
    # print(board)
    board.cards[0].pop()
    board.cards[0].pop()
    board.cards[0].add('EEL')
    board.cards[0].add('COBRA')
    board.cards[1].pop()
    board.cards[1].pop()
    board.cards[1].add('GOOSE')
    board.cards[1].add('CRAB')
    print(board.canMove( board.BLUE, (4,2),"EEL",(3,1)))