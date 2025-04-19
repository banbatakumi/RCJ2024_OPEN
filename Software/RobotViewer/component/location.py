from component.parameters import Params
import math
import tkinter as tk


class Location:
    def __init__(self, main_frame):
        self.main_frame = main_frame
        self.params = Params()

    def create(self):
        pass

    def draw_court(self):
        # 緑のコート
        self.main_frame.create_rectangle(0, 0,
                                         self.params.court_width, self.main_frame.winfo_height(),
                                         width=0, fill="green", tags="court")
        # コートのライン
        self.main_frame.create_rectangle(self.params.court_line_interval, self.params.court_line_interval,
                                         self.params.court_width - self.params.court_line_interval, self.main_frame.winfo_height() -
                                         self.params.court_line_interval,
                                         outline="white", width=self.params.court_line_width, tags="court")
        # コートのゴールライン
        self.main_frame.create_rectangle(self.params.court_half_width - self.params.court_goal_line_width // 2, self.params.court_line_interval,
                                         self.params.court_half_width + self.params.court_goal_line_width // 2, self.params.court_line_interval +
                                         self.params.court_goal_line_height,
                                         outline="white", width=self.params.court_line_width, tags="court")
        # コートのゴールライン
        self.main_frame.create_rectangle(self.params.court_half_width - self.params.court_goal_line_width // 2, self.main_frame.winfo_height() - self.params.court_line_interval - self.params.court_goal_line_height,
                                         self.params.court_half_width + self.params.court_goal_line_width // 2, self.main_frame.winfo_height() -
                                         self.params.court_line_interval,
                                         outline="white", width=self.params.court_line_width, tags="court")
        # コートのゴール
        self.main_frame.create_rectangle(self.params.court_half_width - self.params.court_goal_width // 2, 0,
                                         self.params.court_half_width +
                                         self.params.court_goal_width // 2, self.params.court_goal_height,
                                         width=0, fill="yellow", tags="court")
        # コートのゴール
        self.main_frame.create_rectangle(self.params.court_half_width - self.params.court_goal_width // 2, self.main_frame.winfo_height() - self.params.court_goal_height,
                                         self.params.court_half_width +
                                         self.params.court_goal_width // 2, self.main_frame.winfo_height(),
                                         width=0, fill="blue", tags="court")
        # コートのセンターサークル
        self.main_frame.create_oval(self.params.court_half_width - self.params.court_center_circle_radius, self.center_y - self.params.court_center_circle_radius,
                                    self.params.court_half_width + self.params.court_center_circle_radius, self.center_y +
                                    self.params.court_center_circle_radius,
                                    outline="black", tags="court")

    def draw_ball(self, ball_dir, ball_dis, own_x, own_y):
        ball_x = (ball_dis * self.scale + self.params.robot_radius) * \
            math.sin(math.radians(ball_dir))
        ball_x += self.params.court_half_width + own_x * self.scale
        ball_y = -(ball_dis * self.scale + self.params.robot_radius) * \
            math.cos(math.radians(ball_dir))
        ball_y += self.center_y - own_y * self.scale
        self.main_frame.create_oval(ball_x - self.params.ball_radius, ball_y - self.params.ball_radius,
                                    ball_x + self.params.ball_radius, ball_y + self.params.ball_radius,
                                    width=0, fill="orange", tags="ball")

    def draw_robot(self, own_x, own_y, under_yaw):
        self.main_frame.create_oval(own_x * self.scale + self.params.court_half_width - self.params.robot_radius, -own_y * self.scale + self.center_y - self.params.robot_radius,
                                    own_x * self.scale + self.params.court_half_width + self.params.robot_radius, -
                                    own_y * self.scale + self.center_y + self.params.robot_radius,
                                    fill="gray", tags="robot")

    def update(self, ball_dir, ball_dis, own_x, own_y, under_yaw):
        self.center_x = self.main_frame.winfo_width() // 2  # キャンバスの中央X座標
        self.center_y = self.main_frame.winfo_height() // 2  # キャンバスの中央Y座標
        self.scale = (self.main_frame.winfo_height() /
                      self.params.original_court_height)
        self.params.scale(self.scale)

        self.draw_court()
        self.draw_ball(ball_dir, ball_dis // 2, own_x // 2, own_y // 2)
        self.draw_robot(own_x // 2, own_y // 2, under_yaw)
