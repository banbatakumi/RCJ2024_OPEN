import threading
import tkinter as tk
import component.socket_wifi as socket_wifi

from component.gui import GUI


def main():
    root = tk.Tk()
    root.title("ESP32 Data Monitor")
    root.geometry("700x500")

    gui = GUI(root)

    # スレッド開始
    threading.Thread(target=socket_wifi.socket_thread,
                     args=(gui, root), daemon=True).start()

    # GUIメインループ
    root.mainloop()


if __name__ == "__main__":
    main()
