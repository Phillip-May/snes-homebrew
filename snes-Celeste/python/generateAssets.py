from PIL import Image, ImageDraw, ImageTk
import random
import tkinter as tk
import math
import time
from random import gauss


def generate_cloud_image(width=1024, height=1024, num_clouds=20):
    """
    Generates a rectangle image representing clouds against a black background.
    The clouds should be in 3 different shades of grey. The clouds should not overlap.
    """
    image = Image.new('RGBA', (width, height), (0, 0, 0, 255))
    draw = ImageDraw.Draw(image)
    
    # Define three shades of grey for clouds
    cloud_colors = [
        (64, 64, 64, 255),   # Dark grey
        (128, 128, 128, 255), # Medium grey
        (192, 192, 192, 255)  # Light grey
    ]
    
    # Generate non-overlapping clouds
    clouds = []
    attempts = 0
    max_attempts = num_clouds * 100  # Prevent infinite loops
    
    while len(clouds) < num_clouds and attempts < max_attempts:
        attempts += 1
        
        # Random cloud properties
        cloud_x = random.randint(50, width - 150)
        cloud_y = random.randint(50, height - 150)
        cloud_width = random.randint(90, 350)  # Increased width range for longer clouds
        cloud_height = random.randint(10, 40)
        cloud_color = random.choice(cloud_colors)
        
        # Create cloud rectangle
        cloud_rect = (cloud_x, cloud_y, cloud_x + cloud_width, cloud_y + cloud_height)
        
        # Check for overlap with existing clouds
        overlaps = False
        for existing_cloud in clouds:
            ex_x, ex_y, ex_w, ex_h = existing_cloud
            # Add some padding between clouds
            if (cloud_x < ex_w + 20 and cloud_x + cloud_width > ex_x - 20 and
                cloud_y < ex_h + 20 and cloud_y + cloud_height > ex_y - 20):
                overlaps = True
                break
        
        if not overlaps:
            clouds.append((cloud_x, cloud_y, cloud_x + cloud_width, cloud_y + cloud_height))
            # Draw the cloud as a sharp rectangle (no rounded corners)
            draw.rectangle(cloud_rect, fill=cloud_color)
    
    # Save the image
    image.save('clouds.png')
    print(f"Image generated successfully as 'clouds.png' with {len(clouds)} clouds")
    # Convert to RGB for tkinter compatibility
    return image.convert('RGB')


class ViewportWindow:

    def __init__(self, image):
        self.root = tk.Tk()
        self.root.title("Cloud Viewport")

        # Store the full image
        self.full_image = image
        self.viewport_size = 256
        self.viewport_x = 0
        self.viewport_y = 0
        self.base_scroll_speed = 2.0  # Increased base speed for more noticeable constant movement
        self.min_scroll_speed = self.base_scroll_speed  # Changed to use base speed
        self.max_scroll_speed = 25  # Fast during wind gusts
        self.scroll_direction = 1  # 1 for right, -1 for left
        self.vertical_scroll_direction = 0  # 1 for down, -1 for up
        self.is_scrolling = True

        # Gust timing control
        self.current_time = time.time()
        self.next_gust_time = self.current_time
        self.current_gust_end_time = 0
        self.in_gust = False
        self.gust_transition = 0.0  # 0 to 1 for gust intensity

        # Gust timing parameters (in seconds)
        self.min_pause = 2
        self.max_pause = 10
        self.pause_mean = (self.min_pause + self.max_pause) / 2
        self.pause_std = (self.max_pause - self.min_pause) / 4  # So ±2 std covers the range

        self.min_duration = 2
        self.max_duration = 6
        self.duration_mean = (self.min_duration + self.max_duration) / 2
        self.duration_std = (self.max_duration - self.min_duration) / 4

        # Schedule next gust
        self.schedule_next_gust()

        # Initial viewport position
        self.viewport_y = (self.full_image.height - self.viewport_size) // 2

        # Time tracking for gust transitions
        self.time = 0

        # Create canvas to display image
        self.canvas = tk.Canvas(self.root, width=self.viewport_size, height=self.viewport_size)
        self.canvas.pack()

        # Create initial viewport
        self.update_viewport()

        # Bind keys
        self.root.bind('<Left>', self.start_scroll_left)
        self.root.bind('<Right>', self.start_scroll_right)
        self.root.bind('<Up>', self.start_scroll_up)
        self.root.bind('<Down>', self.start_scroll_down)
        self.root.bind('<space>', self.toggle_scroll)

        # Update viewport every 16ms (approximately 60 FPS)
        self.scroll_viewport()

    def schedule_next_gust(self):
        # Generate pause duration using normal distribution
        pause = gauss(self.pause_mean, self.pause_std)
        # Clamp to min/max range
        pause = max(self.min_pause, min(self.max_pause, pause))

        # Calculate next gust time
        self.next_gust_time = time.time() + pause

        # Generate gust duration using normal distribution
        duration = gauss(self.duration_mean, self.duration_std)
        # Clamp to min/max range
        self.next_gust_duration = max(self.min_duration, min(self.max_duration, duration))

    def update_gust_state(self):
        current_time = time.time()

        if not self.in_gust and current_time >= self.next_gust_time:
            # Start new gust
            self.in_gust = True
            self.current_gust_end_time = current_time + self.next_gust_duration
            print(f"Gust starting! Duration: {self.next_gust_duration:.1f}s")
            # Schedule the next gust
            self.schedule_next_gust()

        if self.in_gust:
            if current_time >= self.current_gust_end_time:
                # End gust
                self.in_gust = False
                print(f"Gust ending. Next gust in: {self.next_gust_time - current_time:.1f}s")
            else:
                # Calculate gust progress (0 to 1)
                progress = (current_time - (self.current_gust_end_time - self.next_gust_duration)) / self.next_gust_duration
                # Create smooth transition using sine wave
                self.gust_transition = math.sin(progress * math.pi)
                # Just update gust transition
                pass
        else:
            self.gust_transition = 0

    def get_current_scroll_speed(self):
        # Update gust state
        self.update_gust_state()

        # Base speed during calm periods (constant slow scroll)
        if not self.in_gust:
            return self.base_scroll_speed

        # During gust, use transition for smooth speed changes
        # Start from base_scroll_speed instead of complete stop
        speed_multiplier = pow(self.gust_transition, 2)  # Square for more pronounced effect
        current_speed = self.base_scroll_speed + (self.max_scroll_speed - self.base_scroll_speed) * speed_multiplier

        return current_speed

    def update_viewport(self):

        # Create a new image for the composite viewport
        viewport = Image.new('RGB', (self.viewport_size, self.viewport_size), 'black')

        # Handle horizontal wrapping
        if self.viewport_x + self.viewport_size > self.full_image.width:
            # Calculate how much extends beyond the right edge
            wrap_amount_x = (self.viewport_x + self.viewport_size) - self.full_image.width
            right_x = self.viewport_x
            left_x = 0
            right_width = self.full_image.width - self.viewport_x
            left_width = wrap_amount_x
        else:
            right_x = self.viewport_x
            right_width = self.viewport_size
            left_x = 0
            left_width = 0

        # Handle vertical wrapping
        if self.viewport_y + self.viewport_size > self.full_image.height:
            # Calculate how much extends beyond the bottom edge
            wrap_amount_y = (self.viewport_y + self.viewport_size) - self.full_image.height
            top_y = self.viewport_y
            bottom_y = 0
            top_height = self.full_image.height - self.viewport_y
            bottom_height = wrap_amount_y
        else:
            top_y = self.viewport_y
            top_height = self.viewport_size
            bottom_y = 0
            bottom_height = 0

        # Get and paste the portions
        if left_width == 0 and bottom_height == 0:
            # No wrapping needed
            portion = self.full_image.crop((right_x, top_y, right_x + right_width, top_y + top_height))
            viewport.paste(portion, (0, 0))
        else:
            # Handle all possible wrap combinations
            if right_width > 0 and top_height > 0:
                portion = self.full_image.crop((right_x, top_y, right_x + right_width, top_y + top_height))
                viewport.paste(portion, (0, 0))
            if left_width > 0 and top_height > 0:
                portion = self.full_image.crop((left_x, top_y, left_x + left_width, top_y + top_height))
                viewport.paste(portion, (right_width, 0))
            if right_width > 0 and bottom_height > 0:
                portion = self.full_image.crop((right_x, bottom_y, right_x + right_width, bottom_y + bottom_height))
                viewport.paste(portion, (0, top_height))
            if left_width > 0 and bottom_height > 0:
                portion = self.full_image.crop((left_x, bottom_y, left_x + left_width, bottom_y + bottom_height))
                viewport.paste(portion, (right_width, top_height))

        # Convert to PhotoImage for Tkinter
        self.tk_image = ImageTk.PhotoImage(viewport)

        # Update canvas
        self.canvas.delete("all")
        self.canvas.create_image(0, 0, anchor=tk.NW, image=self.tk_image)

    def start_scroll_left(self, event=None):
        self.scroll_direction = -1
        self.vertical_scroll_direction = 0
        self.is_scrolling = True

    def start_scroll_right(self, event=None):
        self.scroll_direction = 1
        self.vertical_scroll_direction = 0
        self.is_scrolling = True

    def start_scroll_up(self, event=None):
        self.vertical_scroll_direction = -0.1
        self.is_scrolling = True

    def start_scroll_down(self, event=None):
        self.vertical_scroll_direction = 0.1
        self.is_scrolling = True

    def toggle_scroll(self, event=None):
        self.is_scrolling = not self.is_scrolling

    def scroll_viewport(self):
        if self.is_scrolling:
            # Get current scroll speed
            current_speed = self.get_current_scroll_speed()

            # Update horizontal position with wrapping
            self.viewport_x = int((self.viewport_x + self.scroll_direction * current_speed) % self.full_image.width)
            self.viewport_y = int((self.viewport_y + self.vertical_scroll_direction * current_speed) % self.full_image.height)

            # Update time for gust transitions
            self.time += 1

            self.update_viewport()

        # Schedule next update
        self.root.after(16, self.scroll_viewport)

    def run(self):
        print("Controls:")
        print("Left Arrow: Scroll Left")
        print("Right Arrow: Scroll Right")
        print("Up Arrow: Scroll Up")
        print("Down Arrow: Scroll Down")
        print("Space: Toggle Scrolling")
        self.root.mainloop()

if __name__ == '__main__':
    image = generate_cloud_image()
    window = ViewportWindow(image)
    window.run()
