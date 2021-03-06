#!/usr/bin/env python
#RUBEN CUADRA 
#DAVID GAONA
#code to run a server that manages game boards of the Onitama game
import socket
from time import sleep
from enum import Enum
from threading import Thread
from gui import Onitama_GUI
from onitampy.board import OnitamaBoard
from onitampy.movements import MOVEMENT_ID
from time import sleep
from codes import *
from sys import argv

gui = Onitama_GUI(OnitamaBoard())
START_GUI = 0

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8989        # The port used by the server

#sends toSend and waits for a response, if it matches expectedAnswer it returns True
def send(s,toSend,expectedAnswer=responses.OK):
    s.sendall(toSend) #PVE or PVP - 1 | 0 
    answ = receive(s)
    print(answ)
    return answ == expectedAnswer

def sendAndReturn(s,toSend): 
    s.sendall(toSend) #PVE or PVP - 1 | 0 
    return receive(s)

def receive(s):
    d = s.recv(1024)
    return d.decode('utf-8')[:-1].replace("\x000","").replace("\x001","") if d else None#-1 removes \0

def getModeFromUser():
    print("Select mode:\n\t1)Play Vs Player\n\t2)Play Vs Computer")
    while True:
        a = input("> ")
        if a.isdigit():
            a = int(a)
            if a > 0 and a < 3:
                return options.PVP if a==1 else options.PVE
        print("Wrong option, try again")

def getDifficultyFromUser():
    print("Select difficulty:\n\t1)Normal\n\t2)Hard")
    while True:
        a = input("> ")
        if a.isdigit():
            a = int(a)
            if a > 0 and a < 3:
                return options.DIFFICULTY_NORMAL if a==1 else options.DIFFICULTY_HARD
        print("Wrong option, try again")  

def getTableFromUser():
    print("Select a table id: (-1 for random table, table ids from 0 to 4) ")
    while True:
        a = input("> ")
        if a.isdigit():
            a = int(a)
            if a == -1 or (a >= 0 and a < 5):
                return a
        print("Wrong option, try again")  

class OnitamaClient(Thread):
    def __init__(self):
        ''' Constructor. '''
        Thread.__init__(self)

    def startPVP(self,s,board,we):
        global gui
        playing    = 0
        while not board.isGameOver():
            gui.turn = board.BLUE if playing==0 else board.RED
            if we == playing: 
                print("WE MOVE")
                while True:
                    ans = gui.getSelectedMovement()
                    while ans == None: 
                        sleep(0.1)
                        ans = gui.getSelectedMovement()
                    gui.resetMovement()
                    fr,fc = ans[0]
                    mov_id= MOVEMENT_ID[ans[1]]
                    tr,tc = ans[2]
                    if board.canMove( board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc) ):
                        ans = send(s, ("%s %s %s %s %s"%(fr,fc,tr,tc,mov_id)).encode() )
                        if ans: #SEND IT
                            board = board.move(board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc))
                            playing = (playing+1)%2
                            break
                        else:
                            print("Server error, can't move", ans)  
                    else:
                        print("Local error, can't move")  
            else: #TODO si se desconecta aqui truena, debemos cachar y decir que se fue
                print("WE WAIT")
                fr,fc,tr,tc,mov_id = [int(c) for c in receive(s).split(" ")] #Convert to int
                print('got',fr,fc,tr,tc,mov_id)
                board = board.move( board.BLUE if we!=0 else board.RED , (fr,fc), board.getCardById(mov_id), (tr,tc))
                playing = (playing+1)%2
            gui.board = board
        w = board.getWinner()
        gui.winner = w
        print("Winner is ",'BLUE' if w==board.BLUE else 'RED')

    def run(self): #START_GUI
        global START_GUI,gui
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            #CONNECTION SUCCESFULL, LAUNCH WINDOW
            mode = getModeFromUser()
            if mode == options.PVE: 
                if send(s,mode): 
                    difficulty = getDifficultyFromUser()
                    if send(s,mode):
                        ans = receive(s)
                        if ans:
                            print(ans)
                            ans       = ans.split(" ")
                            we        = int(ans[0]) #0 => BLUE
                            if we != 0: we = 1
                            print("WE ",we)
                            playing   = 0
                            board = OnitamaBoard()
                            gui.player= board.BLUE if we==0 else board.RED
                            gui.board = board
                            gui.turn  = board.BLUE #Blue always starts
                            board.setCardsById(ans[1:]) 
                            START_GUI = 1
                            while not board.isGameOver():
                                if we == playing: 
                                    while True:
                                        ans = gui.getSelectedMovement()
                                        while ans == None: 
                                            sleep(0.25)
                                            ans = gui.getSelectedMovement()
                                        gui.resetMovement()
                                        fr,fc  = ans[0]
                                        mov_id = MOVEMENT_ID[ans[1]]
                                        tr,tc  = ans[2]
                                        if board.canMove( board.BLUE if we==0 else board.RED, (fr,fc), ans[1], (tr,tc) ):
                                            toS = ("%s %s %s %s %s"%(fr,fc,tr,tc,mov_id)).encode()
                                            print("SENDING ",toS,' - ',ans[1])
                                            if send(s,toS): #SEND IT
                                                board = board.move(board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc))
                                                playing = (playing+1)%2
                                                break
                                        print("Wrong movement,try again")    
                                else:
                                    fr,fc,tr,tc,mov_id = [int(c) for c in receive(s).split(" ")] #Convert to int
                                    print("RECV ", fr,fc,tr,tc,mov_id,' - ',board.getCardById(mov_id))
                                    board = board.move( board.BLUE if we==1 else board.RED , (fr,fc), board.getCardById(mov_id), (tr,tc))
                                    playing = (playing+1)%2

                                gui.board = board
                                gui.turn  = board.BLUE if playing==0 else board.RED
                            
                            w = board.getWinner()
                            if w:
                                gui.winner = w
                                print("Winner is ",'BLUE' if w==board.BLUE else 'RED')
                        else:
                            print("ERROR", ans)
                    else:
                        print("ERROR SENDING DIFFICULTY")
            elif mode == options.PVP:
                if send(s,mode): 
                    table = str(getTableFromUser())
                    ans = sendAndReturn(s,table.encode()) #Num Players or ERROR (numeric)
                    while ans.isdigit() and int(ans) == 1: #Just 1 player in table, wait until server sends us something different
                        print("Waiting for a player to join table ",table)
                        ans = receive(s)

                    if (ans.isdigit() and int(ans) == 2) or (' ' in ans): 
                        # GAME STARTS
                        print(ans,repr(ans), type(ans))
                        if ans == '2': #We got number players
                            ans = receive(s) #GET COLOR AND BOARD
                            print("GOT SETUP ",ans) 
                        if ans: #We got setup
                            #Initial SETUP
                            ans        = ans.split(" ")
                            if len(ans) > 6:  ans = ans[len(ans)-6:]
                            we         = int(ans[0]) #0 => BLUE | RED
                            if we != 0: we = 1
                            board      = OnitamaBoard()
                            board.setCardsById(ans[1:]) 
                            #START
                            gui.board = board
                            gui.player = board.BLUE if we==0 else board.RED
                            START_GUI = 1
                            self.startPVP(s,board,we)
                        else:
                            raise Exception("ERROR PLAYING")
                    else: #ERROR
                        print(ans, "Algo salio mal")
            else:
                print("WRONG OPTION, BYE")     
            s.close()

if __name__ == '__main__':

    if len(argv) < 3: print("ERROR, execute as\n\t./client {host_address} {port_number}\n")
    else:
        HOST = argv[1]
        PORT = int(argv[2])
        oc = OnitamaClient()
        oc.setName('COMM Thread')
        oc.start()
        while START_GUI == 0: sleep(0.25) #Wait until COMM Thread tells us to launch it
        if START_GUI == 1:    
            # print("STARTING GUI")
            gui.run()   #Main Thread will be locked here
        oc.join()
        