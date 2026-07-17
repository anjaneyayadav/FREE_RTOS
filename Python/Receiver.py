import socket
import numpy as np
import cv2
import struct
import os
import signal
import sys
from datetime import datetime

HOST = '0.0.0.0'
PORT = 9000
SAVE_DIR = "clips"
FRAME_W, FRAME_H = 320, 240   # QVGA — matches ESP32 FRAMESIZE_QVGA
FPS = 8                        # adjust later if playback looks too fast/slow

os.makedirs(SAVE_DIR, exist_ok=True)

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.bind((HOST, PORT))
server.listen(1)

print("Starting receiver on port 9000...")
print("Waiting for ESP32-CAM to connect...")

writer = None
conn = None
filepath = None
frame_count = 0

def cleanup_and_exit(*args):
    """Ensures the current clip is properly finalized before exiting."""
    global writer, conn
    print("\nShutting down — finalizing any open clip...")
    if writer is not None:
        writer.release()
        writer = None
        print(f"⏹ STREAMING STOPPED — clip saved ({frame_count} frames)")
        if filepath:
            print(f"Saved to: {filepath}")
    if conn is not None:
        conn.close()
    server.close()
    cv2.destroyAllWindows()
    sys.exit(0)

# Catch Ctrl+C and terminal close signals so we always finalize the file
signal.signal(signal.SIGINT, cleanup_and_exit)
signal.signal(signal.SIGTERM, cleanup_and_exit)

try:
    while True:
        conn, addr = server.accept()
        print("=" * 50)
        print("🎥 MOTION DETECTED — STREAMING STARTED")
        print(f"ESP32-CAM connected from {addr}")
        print(f"Time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 50)

        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filepath = os.path.join(SAVE_DIR, f"clip_{timestamp}.mp4")
        fourcc = cv2.VideoWriter_fourcc(*'mp4v')
        writer = cv2.VideoWriter(filepath, fourcc, FPS, (FRAME_W, FRAME_H))
        print(f"Recording to {filepath}")

        frame_count = 0

        try:
            while True:
                # Read 4-byte length header
                header = b''
                while len(header) < 4:
                    chunk = conn.recv(4 - len(header))
                    if not chunk:
                        raise ConnectionError("Disconnected")
                    header += chunk

                frame_len = struct.unpack('<I', header)[0]

                # Sanity check — reject absurd sizes (corrupted header)
                if frame_len <= 0 or frame_len > 5_000_000:
                    raise ConnectionError(f"Invalid frame length: {frame_len}")

                # Read JPEG data
                data = b''
                while len(data) < frame_len:
                    chunk = conn.recv(frame_len - len(data))
                    if not chunk:
                        raise ConnectionError("Disconnected")
                    data += chunk

                # Decode and show
                frame = np.frombuffer(data, dtype=np.uint8)
                img = cv2.imdecode(frame, cv2.IMREAD_COLOR)
                if img is not None:
                    if img.shape[1] != FRAME_W or img.shape[0] != FRAME_H:
                        img = cv2.resize(img, (FRAME_W, FRAME_H))
                    writer.write(img)
                    frame_count += 1
                    cv2.imshow('ESP32-CAM Motion Stream', img)

                if cv2.waitKey(1) & 0xFF == ord('q'):
                    raise KeyboardInterrupt

        except ConnectionError as e:
            print(f"ESP32-CAM disconnected ({e}).")

        finally:
            # This ALWAYS runs, guaranteeing the file gets finalized
            if writer is not None:
                writer.release()
                writer = None
                print("=" * 50)
                print(f"⏹ STREAMING STOPPED — clip saved ({frame_count} frames)")
                print(f"Saved to: {filepath}")
                print("=" * 50)
            if conn is not None:
                conn.close()
                conn = None

        print("Waiting for next motion event...")

except KeyboardInterrupt:
    cleanup_and_exit()

finally:
    server.close()
    cv2.destroyAllWindows()