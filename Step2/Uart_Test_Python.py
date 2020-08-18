from tkinter import *
from tkinter import ttk
from tkinter import messagebox
import serial
import binascii


def send():
    RX_list = []

    input_str = send_str.get()

    hex_number = []

    for i in list(input_str):

        if radvar.get() == 1:
            ser.write(bytes(i, encoding='ascii'))
            TX_Lable.configure(text=input_str)
            RX_list.append(ser.read().decode())

        elif radvar.get() == 2:
            ser.write(bytes(i, encoding='ascii'))
            hex_number.append('0x%x' % ord(i))
            TX_Lable.configure(text=hex_number)
            RX_list.append('0x%x' % ord(ser.read().decode()))

    RX_Lable.configure(text=str(RX_list))


def Open_Port():
    global ser

    open_port.config(bd=0, fg='gray', state='disabled')

    close_port.config(bd=3, fg='black', state='normal')

    send.config(bd=3, fg='black', state='normal')

    if Parity.get() == 'None':

        Parity_Bit = serial.PARITY_NONE

    elif Parity.get() == 'Odd':

        Parity_Bit = serial.PARITY_ODD

    else:

        Parity_Bit = serial.PARITY_EVEN

    if Stop_Bit.get() == '1':

        STOP_BITS = serial.STOPBITS_ONE

    else:

        STOP_BITS = serial.STOPBITS_TWO

    ser = serial.Serial(USB_port.get(), int(BaudRate.get()), bytesize=int(Data_Bit.get()),
                        parity=Parity_Bit,stopbits=STOP_BITS)

def Close_Port():
    open_port.config(bd=3, fg='black', state='normal')

    close_port.config(bd=0, fg='gray', state='disabled')

    send.config(bd=0, fg='gray', state='disabled')


if __name__ == '__main__':
    root = Tk()
    root.title('Serial Program')
    root.geometry("700x400+100+100")

    send_str = StringVar()

    frame1 = Frame(root, relief="solid", bd=1)
    frame1.pack(side='left', fill='both')

    frame2 = Frame(root, relief="solid", bd=1)
    frame2.pack(side='top', fill='both')

    frame4 = Frame(root, relief='solid', bd=1, width=200, height=10)
    frame4.pack(side='bottom', fill='both', expand=True)

    frame3 = Frame(root, relief='solid', bd=1, width=200, height=10)
    frame3.pack(side='bottom', fill='both', expand=True)

    # frame1
    USB_config = Label(frame1, text='USB Config', font=5)
    USB_config.pack(anchor='c')
    Port_label = Label(frame1, text='USB_Port')
    Port_label.pack()
    USB_port = ttk.Combobox(frame1,
                            values=["/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyUSB0", "/dev/ttyUSB3", "/dev/ttyUSB4"])

    USB_port.current(0)
    USB_port.pack()

    BR_label = Label(frame1, text='BaudRate')
    BR_label.pack()

    BaudRate = ttk.Combobox(frame1, values=["4800", "9600", "19200", "38400", "57600", "115200"])
    BaudRate.current(1)
    BaudRate.pack()

    Data_label = Label(frame1, text='Data Bit')
    Data_label.pack()

    Data_Bit = ttk.Combobox(frame1, values=["5", "6", "7", "8"])
    Data_Bit.current(3)
    Data_Bit.pack()

    SP_label = Label(frame1, text='Stop Bit')
    SP_label.pack()

    Stop_Bit = ttk.Combobox(frame1, values=["1", "2"])
    Stop_Bit.current(0)
    Stop_Bit.pack()

    Pry_label = Label(frame1, text='Parity Bit')
    Pry_label.pack()

    Parity = ttk.Combobox(frame1, values=["None", "Even", "Odd"])
    Parity.current(0)
    Parity.pack()

    open_port = Button(frame1, text="open port", command=Open_Port, width=8, bd=3)
    open_port.pack(side='left')

    close_port = Button(frame1, text="close port", width=8, command=Close_Port, bd=0,
                        fg='gray',state='disabled')

    close_port.pack(side='left')
    radvar = IntVar()

    # frame2
    encoding_sc = StringVar()
    encoding_AC = ttk.Radiobutton(frame2, text="ASCII", variable=radvar, value=1)  # ,varialbe = encoding_sc)
    encoding_AC.pack(side='left')
    encoding_HX = Radiobutton(frame2, text="HEX", variable=radvar, value=2)
    encoding_HX.pack(side='left')

    ent = Entry(frame2, width=43, textvariable=send_str)
    ent.pack(side='left')

    send = Button(frame2, text='send', command=send, bd=0, fg='gray', state='disabled')
    send.pack(side='left')

    # frame4
    RX = Label(frame4, text='Serial RX', font=5)
    RX.pack(anchor='w')
    RX_Lable = Label(frame4, text='')
    RX_Lable.pack(anchor='w')

    # frame3
    TX = Label(frame3, text='Serial TX', font=5)
    TX.pack(anchor='w')
    TX_Lable = Label(frame3, text='')
    TX_Lable.pack(anchor='w')

    root.mainloop()
