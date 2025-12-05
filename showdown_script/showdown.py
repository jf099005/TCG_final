#!/usr/bin/env python
import subprocess
import multiprocessing
import argparse
import sys
import re
from pathlib import Path

def play_one_game(args):
    referee_path, agent1_path, agent2_path, show_detail, game_id = args

    def readline(proc):
        """Read line, safely."""
        line = proc.stdout.readline()
        if not line:
            return None
        return line.strip()

    try:
        # Start referee and agents
        ref = subprocess.Popen([referee_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        a1 = subprocess.Popen([agent1_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        a2 = subprocess.Popen([agent2_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)

        ply = 1

        #print(f">r 'START 0'")
        ref.stdin.write("START 1\n") # dark chess
        ref.stdin.flush()

        while True:
            msg = readline(ref)
            #print(f"<r '{msg}'")
            if msg is None or msg.startswith("ERR"):
                break

            red = a1 if (game_id % 2) else a2
            black = a2 if (game_id % 2) else a1
            red_str = "Agent 1" if (game_id % 2) else "Agent 2"
            black_str = "Agent 2" if (game_id % 2) else "Agent 1"

            # get state
            #print(f">r 'STATE'")
            ref.stdin.write("STATE\n")
            ref.stdin.flush()
            status = readline(ref)
            #print(f"<r '{status}'")
            if status == "IN-PLAY":
                board = readline(ref)
                ok = readline(ref)

            elif status == "RED WINS":
                board = readline(ref)
                reason = readline(ref)
                if show_detail:
                    print(f"Game #{game_id}: \033[1;31m{red_str}\033[0m won by {reason}")
                return red_str, 'r'

            elif status == "BLACK WINS":
                board = readline(ref)
                reason = readline(ref)
                if show_detail:
                    print(f"Game #{game_id}: \033[1;30m{black_str}\033[0m won by {reason}")
                return black_str, 'b'

            elif status == "DRAW":
                board = readline(ref)
                reason = readline(ref)
                if show_detail:
                    print(f"Game #{game_id}: \033[1;32mdrawn\033[0m by {reason}")
                return "draw", 'd'

            else:
                break

            # even numbered games: a2 first
            to_play = red if (ply % 2) else black

            # check agent status
            if to_play.poll() is not None:
                if show_detail:
                    print(f"Game #{game_id}: {red_str if (ply % 2) else black_str} crashed")
                return black_str, 'b' if (ply % 2) else red_str, 'r'

            # get moves from agent
            #print(f">a '{board}'")
            to_play.stdin.write(board + '\n')
            to_play.stdin.flush()
            do_move = readline(to_play)
            #print(f"<a '{do_move}'")

            if do_move is None:
                if show_detail:
                    print(f"Game #{game_id}: {red_str if (ply % 2) else black_str} did not respond.")
                return black_str, 'b' if (ply % 2) else red_str, 'r'

            # submit move
            #print(f">r '{do_move}'")
            ref.stdin.write(do_move + '\n')
            ref.stdin.flush()

            # increment
            ply += 1

        ref.kill(); a1.kill(); a2.kill()
        return "error", ''

    except Exception as e:
        return f"error: {e}", ''

def main():
    parser = argparse.ArgumentParser(description="Run multiple games between two agents using a referee.")
    parser.add_argument("referee", type=Path, help="Path to referee executable")
    parser.add_argument("agent1", type=Path, help="Path to first agent")
    parser.add_argument("agent2", type=Path, help="Path to second agent")
    parser.add_argument("-n", "--num-games", type=int, default=10)
    parser.add_argument("-j", "--jobs", type=int, default=multiprocessing.cpu_count())
    parser.add_argument("-s", "--show-detail", type=int, default=1, help="Show results of each match.")
    args = parser.parse_args()

    jobs = [(str(args.referee), str(args.agent1), str(args.agent2), args.show_detail, i) for i in range(args.num_games)]

    with multiprocessing.Pool(args.jobs) as pool:
        results = pool.map(play_one_game, jobs)

    counts = {}
    rsplit = {'Agent 1': 0, 'Agent 2': 0}
    for r, s in results:
        counts[r] = counts.get(r, 0) + 1
        if r == 'Agent 1' or r == 'Agent 2':
            if s == 'r':
                rsplit[r] += 1

    print(f"=== Results over {args.num_games} games ===")
    for k, v in counts.items():
        print(f"  {k}: {v} {f"({rsplit[k]} Red)" if k in rsplit else ''}")

    print("==============================")
    a2_score = counts.get('Agent 2', 0) + 0.5 * counts.get('draw', 0)
    a1_score = counts.get('Agent 1', 0) + 0.5 * counts.get('draw', 0)
    print(f"{'Agent 1':<10}{a1_score} - {a2_score}{'Agent 2':>10}")

if __name__ == "__main__":
    main()