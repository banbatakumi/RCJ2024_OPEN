class Params:
    def __init__(self):
        # オリジナルデータを保持
        self.original_robot_diameter = 180  # ロボットの直径（mm）
        self.original_ball_diameter = 45  # ボールの直径（mm）
        self.original_court_width = 1820  # コートの幅（mm）
        self.original_court_height = 2430  # コートの長さ（mm）
        self.original_court_line_width = 20
        self.original_court_line_interval = 120
        self.original_court_goal_width = 600
        self.original_court_goal_height = 140
        self.original_court_center_circle_radius = 300
        self.original_court_goal_line_width = 800
        self.original_court_goal_line_height = 250

        # スケール後の値（初期値はオリジナルと同じ）
        self.robot_diameter = self.original_robot_diameter
        self.ball_diameter = self.original_ball_diameter
        self.ball_radius = self.ball_diameter // 2
        self.robot_radius = self.robot_diameter // 2
        self.court_width = self.original_court_width
        self.court_height = self.original_court_height
        self.court_half_width = self.original_court_width // 2
        self.court_half_height = self.original_court_height // 2
        self.court_line_width = self.original_court_line_width
        self.court_line_interval = self.original_court_line_interval
        self.court_goal_width = self.original_court_goal_width
        self.court_goal_height = self.original_court_goal_height
        self.court_center_circle_radius = self.original_court_center_circle_radius
        self.court_goal_line_width = self.original_court_goal_line_width
        self.court_goal_line_height = self.original_court_goal_line_height

    def scale(self, scale_factor):
        """スケールを適用して値を更新"""
        self.robot_diameter = self.original_robot_diameter * scale_factor
        self.ball_diameter = self.original_ball_diameter * scale_factor
        self.ball_radius = self.ball_diameter // 2
        self.robot_radius = self.robot_diameter // 2
        self.court_width = self.original_court_width * scale_factor
        self.court_height = self.original_court_height * scale_factor
        self.court_half_width = self.original_court_width * scale_factor // 2
        self.court_half_height = self.original_court_height * scale_factor // 2
        self.court_line_width = self.original_court_line_width * scale_factor
        self.court_line_interval = self.original_court_line_interval * scale_factor
        self.court_goal_width = self.original_court_goal_width * scale_factor
        self.court_goal_height = self.original_court_goal_height * scale_factor
        self.court_center_circle_radius = self.original_court_center_circle_radius * scale_factor
        self.court_goal_line_width = self.original_court_goal_line_width * scale_factor
        self.court_goal_line_height = self.original_court_goal_line_height * scale_factor
