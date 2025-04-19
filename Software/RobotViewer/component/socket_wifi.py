import socket
import time
from pynput import keyboard
import threading

# 接続情報
HOST = '192.168.4.1'  # ESP32のIPアドレス
PORT = 1234  # ESP32の送信ポート

# キーと角度の対応表
KEY_TO_ANGLE = {
    'w': 0,    # 前進
    'a': -90,   # 左に回転
    's': 180,  # 後退
    'd': 90   # 右に回転
}

# 特定のキーの組み合わせに対応する角度
COMBO_TO_ANGLE = {
    frozenset(['w', 'd']): 45,    # WDが押された場合
    frozenset(['w', 'a']): -45,   # WAが押された場合
    frozenset(['s', 'd']): 135,   # SDが押された場合
    frozenset(['s', 'a']): -135,  # SAが押された場合
}

TOP_SPEED = 1
MID_SPEED = 0.7
LOW_SPEED = 0.4

FACE_ANGLE = 30


def socket_thread(gui, root):
    pressed_keys = set()  # 現在押されているキーを追跡するセット

    dribbler_active = False  # ドリブラーの状態を追跡
    speed = MID_SPEED

    def send_command(s, command):
        """ロボットに角度を送信する"""
        try:
            s.sendall(command.encode())
            print(f"コマンド送信: {command}")
        except Exception as e:
            print("コマンド送信エラー:", e)

    def handle_key_input(s):
        """キー入力を処理するスレッド"""
        def on_press(key):
            nonlocal dribbler_active
            """キー押下時の処理"""
            try:
                if hasattr(key, 'char') and key.char.lower() in KEY_TO_ANGLE:
                    pressed_keys.add(key.char.lower())  # 押されたキーを追跡
                    # 現在のキーの組み合わせを確認
                    combo = frozenset(pressed_keys)
                    if combo in COMBO_TO_ANGLE:
                        angle = COMBO_TO_ANGLE[combo]
                    else:
                        angle = KEY_TO_ANGLE[key.char.lower()]
                    send_command(s, f"MOVE:{angle}\n")
                elif key == keyboard.Key.space:
                    send_command(s, "KICK\n")
                elif key == keyboard.Key.shift:
                    dribbler_active = True
                    send_command(s, "DRIBBLER:ON\n")
                elif key == keyboard.Key.up:
                    speed = TOP_SPEED
                    send_command(s, f"SPEED:{speed}\n")
                elif key == keyboard.Key.down:
                    speed = LOW_SPEED
                    send_command(s, f"SPEED:{speed}\n")
                elif key == keyboard.Key.left:
                    send_command(s, f"FACE:{FACE_ANGLE * -1}\n")
                elif key == keyboard.Key.right:
                    send_command(s, f"FACE:{FACE_ANGLE}\n")
            except AttributeError:
                pass

        def on_release(key):
            nonlocal dribbler_active
            """キーリリース時の処理"""
            try:
                if hasattr(key, 'char') and key.char.lower() in pressed_keys:
                    pressed_keys.remove(key.char.lower())
                    # 全てのキーが離された場合にSTOPを送信
                    if not pressed_keys:
                        send_command(s, "STOP\n")
                    else:
                        # 他のキーがまだ押されている場合に対応する角度を送信
                        combo = frozenset(pressed_keys)
                        if combo in COMBO_TO_ANGLE:
                            angle = COMBO_TO_ANGLE[combo]
                        elif len(pressed_keys) == 1:
                            angle = KEY_TO_ANGLE[next(iter(pressed_keys))]
                        else:
                            return
                        send_command(s, f"MOVE:{angle}\n")
                elif key == keyboard.Key.shift:
                    dribbler_active = False
                    send_command(s, "DRIBBLER:OFF\n")
                elif key == keyboard.Key.up or key == keyboard.Key.down:
                    speed = MID_SPEED
                    send_command(s, f"SPEED:{speed}\n")
                elif key == keyboard.Key.left or key == keyboard.Key.right:
                    send_command(s, f"FACE:{0}\n")
            except AttributeError:
                pass

        # キーボードリスナーを開始
        with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
            listener.join()

    while True:
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.settimeout(3.0)  # 3秒間データが来なければタイムアウト
                print("ESP32に接続を試みています")
                s.connect((HOST, PORT))
                print("ESP32に接続しました")
                gui.update_connection_status(True)

                # キー入力処理を別スレッドで開始
                key_thread = threading.Thread(
                    target=handle_key_input, args=(s,), daemon=True)
                key_thread.start()

                buffer = ""
                while True:
                    try:
                        chunk = s.recv(1024).decode()
                        if not chunk:
                            print("通信が途絶えました")
                            break
                        buffer += chunk
                        while "\n" in buffer:
                            line, buffer = buffer.split("\n", 1)
                            data_list = line.strip().split(",")
                            root.after(0, gui.update_gui, data_list)
                    except socket.timeout:
                        print("一定時間データが受信されませんでした")
                        gui.update_connection_status(False)
                        break
        except Exception as e:
            print("通信エラー:", e)
            gui.update_connection_status(False)
            time.sleep(1)  # 再接続までの待機時間
            gui.update_gui(data_list=["0"] * 16)
