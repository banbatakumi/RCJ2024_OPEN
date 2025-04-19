import tkinter as tk
from component.location import Location
from component.camera import Camera
from component.monitor import Monitor


class GUI:
    def __init__(self, root):
        # 上部のステータスバー
        self.status_frame = tk.Frame(root, bg="lightgrey", height=50)
        self.status_frame.pack(side="top", fill="x")

        # ESP32接続状態ラベル
        self.connection_status = tk.StringVar(value="未接続")
        self.connection_label = tk.Label(self.status_frame, textvariable=self.connection_status,
                                         font=("Arial", 15), fg="red", bg="lightgrey")
        self.connection_label.pack(side="left", padx=10, pady=5)

        # バッテリー残量表示ラベル
        self.battery_percentage = tk.StringVar(value="残量: ---%")
        self.battery_label = tk.Label(self.status_frame, textvariable=self.battery_percentage,
                                      font=("Arial", 15), fg="green", bg="lightgrey")
        self.battery_label.pack(side="left", padx=10, pady=5)

        # 電圧表示ラベル
        self.voltage_value = tk.StringVar(value="電圧: --- V")
        self.voltage_label = tk.Label(self.status_frame, textvariable=self.voltage_value,
                                      font=("Arial", 15), fg="blue", bg="lightgrey")
        self.voltage_label.pack(side="left", padx=10, pady=5)

        # 左側のボタンフレーム
        self.button_frame = tk.Frame(root, bg="gray")
        self.button_frame.pack(side="left", fill="y")

        # メインフレーム
        self.main_frame = tk.Canvas(root, bg="black")
        self.main_frame.pack(side="right", fill="both", expand=True)

        # ボタンの作成
        buttons = ["Location", "Camera", "Monitor"]
        button_widgets = {}
        frame_name = None
        for button in buttons:
            btn = tk.Button(self.button_frame, text=button, command=lambda b=button:
                            self.show_frame(b), font=("Arial", 20), width=6, height=2)
            btn.pack(fill="x", padx=5, pady=5)
            button_widgets[button] = btn

        self.frame_type = None

        self.location = Location(self.main_frame)
        self.camera = Camera(self.main_frame)
        self.monitor = Monitor(self.main_frame)

    def show_frame(self, frame_name):
        frame_type = frame_name
        # メインフレーム内のすべてのウィジェットを削除
        for widget in self.main_frame.winfo_children():
            widget.destroy()

        # キャンバス内の描画要素を削除
        self.frame_type = frame_name
        if (frame_name == "Location"):
            self.location.create()
        if (frame_name == "Camera"):
            self.camera.create()
        elif (frame_name == "Monitor"):
            self.monitor.create()

    def update_connection_status(self, status):
        """ESP32の接続状態を更新"""
        if status:
            self.connection_status.set("接続中")
            self.connection_label.config(fg="green")
        else:
            self.connection_status.set("未接続")
            self.connection_label.config(fg="red")

    # バッテリー残量計算関数
    def calculate_battery_percentage(self, voltage):
        """電圧値からバッテリー残量を計算"""
        min_voltage = 9.0  # 最低電圧
        max_voltage = 12.6  # 最大電圧
        percentage = (voltage - min_voltage) / \
            (max_voltage - min_voltage) * 100
        return max(0, min(100, percentage))  # 0%〜100%の範囲に制限

    def update_gui(self, data_list):
        try:
            voltage = float(data_list[0])
            moving_dir = float(data_list[1])
            moving_dis = float(data_list[2])
            own_x = float(data_list[3]) * 10
            own_y = float(data_list[4]) * 10
            under_yaw = float(data_list[5])
            ball_dir = float(data_list[6])
            ball_dis = float(data_list[7]) * 10  # mm
            yellow_goal_dir = float(data_list[8])
            yellow_goal_dis = float(data_list[9]) * 10
            blue_goal_dir = float(data_list[10])
            blue_goal_dis = float(data_list[11]) * 10
            is_ball_hold_front = int(data_list[12])
            is_ball_hold_back = int(data_list[13])
            line_inside_dir = float(data_list[14])
            line_depth = float(data_list[15])

            # バッテリー残量ラベルを更新
            percentage = self.calculate_battery_percentage(voltage)
            self.battery_percentage.set(f"残量: {percentage:.0f}%")

            # 電圧ラベルを更新
            self.voltage_value.set(f"電圧: {voltage:.2f} V")

            self.main_frame.delete("all")
            if self.frame_type == "Location":
                self.location.update(ball_dir, ball_dis,
                                     own_x, own_y, under_yaw)
            if self.frame_type == "Camera":
                self.camera.update(ball_dir, ball_dis, yellow_goal_dir, yellow_goal_dis,
                                   blue_goal_dir, blue_goal_dis)
            elif self.frame_type == "Monitor":
                self.monitor.update(data_list)
        except Exception as e:
            print("データ更新エラー:", e)
