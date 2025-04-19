from component.parameters import Params
import math
import tkinter as tk


class Camera:
    def __init__(self, main_frame):
        self.main_frame = main_frame
        self.params = Params()

    def create(self):
        pass

    def draw_ball(self, ball_dir, ball_dis):
        ball_x = (ball_dis * self.scale) * math.sin(math.radians(ball_dir))
        ball_x += self.center_x
        ball_y = -(ball_dis * self.scale) * math.cos(math.radians(ball_dir))
        ball_y += self.center_y
        self.main_frame.create_oval(ball_x - self.params.ball_radius, ball_y - self.params.ball_radius,
                                    ball_x + self.params.ball_radius, ball_y + self.params.ball_radius,
                                    width=0, fill="orange", tags="camera")

    def draw_goal(self, dir, dis, color):
        goal = [[self.params.court_goal_width // 2, -dis * self.scale - self.params.court_goal_height],
                [-self.params.court_goal_width // 2, -dis *
                    self.scale - self.params.court_goal_height],
                [-self.params.court_goal_width // 2, -dis * self.scale],
                [self.params.court_goal_width // 2, -dis * self.scale]]
        # ゴールの回転
        for i in range(len(goal)):
            x, y = goal[i]
            rotated_x = x * math.cos(math.radians(dir)) - \
                y * math.sin(math.radians(dir))
            rotated_y = x * math.sin(math.radians(dir)) + \
                y * math.cos(math.radians(dir))
            goal[i] = [self.center_x + rotated_x, self.center_y + rotated_y]
        self.main_frame.create_polygon(goal, fill=color, tags="camera")

    def update(self, ball_dir, ball_dis, yellow_goal_dir, yellow_goal_dis,
               blue_goal_dir, blue_goal_dis):
        self.center_x = self.main_frame.winfo_width() // 2  # キャンバスの中央X座標
        self.center_y = self.main_frame.winfo_height() // 2  # キャンバスの中央Y座標
        self.scale = (self.main_frame.winfo_height() / 2000)
        self.params.scale(self.scale)

        radius = self.main_frame.winfo_height() // 2
        self.main_frame.create_oval(self.center_x - radius, self.center_y - radius,
                                    self.center_x + radius, self.center_y + radius,
                                    fill="green", tags="camera")
        self.main_frame.create_line(self.center_x - radius, self.center_y, self.center_x + radius, self.center_y,
                                    fill="white", tags="camera")
        self.main_frame.create_line(self.center_x, 0, self.center_x, self.main_frame.winfo_height(),
                                    fill="white", tags="camera")

        self.draw_ball(ball_dir, ball_dis // 2)
        self.draw_goal(yellow_goal_dir, yellow_goal_dis // 2, "yellow")
        self.draw_goal(blue_goal_dir, blue_goal_dis // 2, "blue")
