
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter as tk


class Monitor:
    def __init__(self, main_frame):
        # データ名リスト
        self.data_names = [
            "voltage", "moving_dir", "moving_dis", "own_x", "own_y", "under_yaw",
            "ball_dir", "ball_dis", "yellow_goal_dir", "yellow_goal_size",
            "blue_goal_dir", "blue_goal_size", "is_ball_hold_front",
            "is_ball_hold_back", "line_inside_dir", "line_depth"
        ]

        # グローバル変数
        self.serial_monitor = None
        self.graph_data = []
        self.selected_data_index = tk.StringVar()  # プルダウンで選択されたデータの変数名
        self.fig, self.ax = None, None
        self.canvas = None
        self.main_frame = main_frame

    def create(self):
        """UI要素を初期化時に一度だけ作成"""
        # シリアルモニター用フレーム
        self.monitor_frame = tk.Frame(self.main_frame, bg="white")
        self.monitor_frame.place(relx=0, rely=0, relwidth=1, relheight=0.5)

        # シリアルモニター用のTextウィジェット
        self.serial_monitor = tk.Text(
            self.monitor_frame, height=10, state="disabled", bg="black", fg="white", font=("Courier", 10))
        self.serial_monitor.pack(side="top", fill="both", expand=True)

        # データ選択用のプルダウンボックス
        self.dropdown_frame = tk.Frame(self.monitor_frame, bg="lightgrey")
        self.dropdown_frame.pack(side="bottom", fill="x")
        tk.Label(self.dropdown_frame, text="表示するデータ:",
                 bg="lightgrey").pack(side="left", padx=5)
        self.dropdown = tk.OptionMenu(
            self.dropdown_frame, self.selected_data_index, *self.data_names)
        self.dropdown.pack(side="left", padx=5)

        # グラフ表示用フレーム
        self.graph_frame = tk.Frame(self.main_frame, bg="white")
        self.graph_frame.place(relx=0, rely=0.5, relwidth=1, relheight=0.5)

        # Matplotlibのグラフを作成
        self.fig, self.ax = plt.subplots()
        self.ax.set_title("データプロット")
        self.ax.set_xlabel("時間")
        self.ax.set_ylabel("値")
        self.line, = self.ax.plot([], [], "r-")  # 空のグラフを作成
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.graph_frame)
        self.canvas.get_tk_widget().pack(fill="both", expand=True)

    def update(self, data_list):

        # # グラフを更新
        try:
            selected_name = self.selected_data_index.get()  # 選択された変数名
            index = self.data_names.index(selected_name)  # 変数名に対応するインデックスを取得
            self.serial_monitor.config(state="normal")
            self.serial_monitor.insert("end", data_list[index] + "\n")
            self.serial_monitor.see("end")
            self.serial_monitor.config(state="disabled")

            if 0 <= index < len(data_list):
                # 数値に変換可能か確認
                try:
                    value = float(data_list[index])
                    self.graph_data.append(value)
                    if len(self.graph_data) > 100:  # 最大100点まで保持
                        self.graph_data.pop(0)
                    self.line.set_data(
                        range(len(self.graph_data)), self.graph_data)  # データを更新
                    self.ax.relim()  # 軸の範囲を再計算
                    self.ax.autoscale_view()  # 自動スケーリング
                    self.canvas.draw_idle()  # 再描画をスケジュール
                except ValueError:
                    print(f"無効なデータ: {data_list[index]}")
        except Exception as e:
            print("グラフ更新エラー:", e)
