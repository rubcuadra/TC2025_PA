#!/usr/bin/env python3
import socket
from time import sleep
from enum import Enum
from threading import Thread
from gui import Onitama_GUI
from onitampy.board import OnitamaBoard
from time import sleep
from codes import *

gui = Onitama_GUI(OnitamaBoard())
START_GUI = 0

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8989        # The port used by the server

#sends toSend and waits for a response, if it matches expectedAnswer it returns True
def send(s,toSend,expectedAnswer=responses.OK):
    s.sendall(toSend) #PVE or PVP - 1 | 0 
    return receive(s) == expectedAnswer

def sendAndReturn(s,toSend): 
    s.sendall(toSend) #PVE or PVP - 1 | 0 
    return receive(s)

def receive(s):
    d = s.recv(1024)
    return d.decode('utf-8')[:-1] if d else None#-1 removes \0

class OnitamaClient(Thread):
    def __init__(self):
        ''' Constructor. '''
        Thread.__init__(self)

    def run(self): #START_GUI
        global START_GUI,gui
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            #CONNECTION SUCCESFULL, LAUNCH WINDOW
            START_GUI = True
            
            while(gui.mode == None): sleep(0.25) #Wait until player picks
            mode = gui.mode 
            if mode == options.PVE: 
                if send(s,options.PVE): 
                    if send(s,gui.difficulty): #NORMAL
                        ans = receive(s)
                        if ans:
                            ans       = ans.split(" ")
                            we        = int(ans[0]) #0 => BLUE
                            playing   = 0
                            board = OnitamaBoard()
                            gui.player= board.BLUE if we==0 else board.RED
                            board.setCardsById(ans[1:]) 
                            gui.board = board
                            while not board.isGameOver():
                                gui.turn = board.BLUE if playing==0 else board.RED
                                if we == playing: 
                                    while True:
                                        
                                        ans = gui.getSelectedMovement()
                                        while ans == None: 
                                            sleep(0.25)
                                            ans = gui.getSelectedMovement()
                                        gui.resetMovement()
                                        fr,fc = ans[0]
                                        tr,tc = ans[2]
                                        
                                        if board.canMove( board.BLUE if we==0 else board.RED, (fr,fc), ans[1], (tr,tc) ):
                                            if send(s,f"{fr} {fc} {tr} {tc} {mov_id}".encode()): #SEND IT
                                                board = board.move(board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc))
                                                playing = (playing+1)%2
                                                break
                                        print("Wrong movement,try again")    
                                else:
                                    fr,fc,tr,tc,mov_id = [int(c) for c in receive(s).split(" ")] #Convert to int
                                    board = board.move( board.BLUE if we==1 else board.RED , (fr,fc), board.getCardById(mov_id), (tr,tc))
                                    playing = (playing+1)%2
            elif mode == options.PVP:
                while True:
                    if send(s,options.PVP): 
                        table = gui.table
                        while table<-1 or table>5: 
                            sleep(0.25)
                            table = gui.table
                        if table.isdigit(): 
                            ans = int(sendAndReturn(s,table.encode())) #Num Players or ERROR (numeric)
                            while ans == 1: #Just 1 player in table, wait until server sends us something different
                                print(ans)
                                ans = int(receive(s))

                            if ans == 2: 
                                # GAME STARTS
                                ans = receive(s) #GET COLOR AND BOARD
                                if ans:
                                    #Initial SETUP
                                    ans     = ans.split(" ")
                                    we      = int(ans[0]) #0 => BLUE
                                    playing = 0
                                    board   = OnitamaBoard()
                                    board.setCardsById(ans[1:]) 
                                    #START
                                    while not board.isGameOver():
                                        print(board) #TODO draw instead of print
                                        if we == playing: 
                                            while True:
                                                print(f"YOUR TURN: {'BLUE' if we==0 else 'RED' }")
                                                print("Fr fc tr tc movement")
                                                text = input("> ")
                                                ans = text.split(" ")
                                                if len(ans)==5: 
                                                    fr,fc,tr,tc,mov_id = [int(c) for c in ans] #Convert to int
                                                    print(board.getCardById(mov_id))
                                                    if board.canMove( board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc) ):
                                                        if send(s,f"{fr} {fc} {tr} {tc} {mov_id}".encode()): #SEND IT
                                                            board = board.move(board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc))
                                                            playing = (playing+1)%2
                                                            break
                                                print("Wrong movement,try again")    
                                        else: #TODO si se desconecta aqui truena, debemos cachar y decir que se fue
                                            fr,fc,tr,tc,mov_id = [int(c) for c in receive(s).split(" ")] #Convert to int
                                            board = board.move( board.BLUE if we==1 else board.RED , (fr,fc), board.getCardById(mov_id), (tr,tc))
                                            playing = (playing+1)%2

                                else:
                                    raise Exception("ERROR PLAYING")
                                
                            else: #ERROR
                                print(ans, "Algo salio mal")
                        print("Wrong table, try again")
            else:
                print("WRONG OPTION, BYE")     
            s.close()

if __name__ == '__main__':
    oc = OnitamaClient()
    oc.setName('COMM Thread')
    oc.start()
    while START_GUI == 0: sleep(0.25) #Wait until COMM Thread tells us to launch it
    if START_GUI == 1:    gui.run()   #Main Thread will be locked here
    oc.join()
    