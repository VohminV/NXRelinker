#!/usr/bin/env python3
"""Report HTTP reachability of URLs registered in REFERENCE_SOURCES.md.

This is a research aid, not a build gate. Some sources intentionally block
automated clients or are retained because they are historical and unavailable.
"""

from __future__ import annotations

import argparse
import concurrent.futures
import re
import sys
import urllib.error
import urllib.request
from pathlib import Path

URL_PATTERN = re.compile(r"https?://[^\s<>|)]+")
DEFAULT_DOCUMENT = Path("docs/research/REFERENCE_SOURCES.md")


def collect_urls(document: Path) -> list[str]:
    text = document.read_text(encoding="utf-8")
    urls = {match.rstrip(".,;") for match in URL_PATTERN.findall(text)}
    return sorted(urls)


def check(url: str, timeout: float) -> tuple[str, str, str]:
    request = urllib.request.Request(
        url,
        headers={
            "User-Agent": "NXRelinker-source-check/1.0 (+research; contact repository maintainers)",
            "Accept": "text/html,application/json,*/*",
        },
        method="GET",
    )
    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            return url, str(response.status), response.geturl()
    except urllib.error.HTTPError as error:
        return url, str(error.code), error.geturl()
    except Exception as error:  # noqa: BLE001 - report all network failures
        return url, "ERROR", f"{type(error).__name__}: {error}"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--document", type=Path, default=DEFAULT_DOCUMENT)
    parser.add_argument("--timeout", type=float, default=25.0)
    parser.add_argument("--workers", type=int, default=12)
    args = parser.parse_args()

    if not args.document.exists():
        print(f"missing source document: {args.document}", file=sys.stderr)
        return 2

    urls = collect_urls(args.document)
    print(f"checking {len(urls)} URLs from {args.document}")

    failures = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.workers) as executor:
        for url, status, final in sorted(executor.map(lambda u: check(u, args.timeout), urls)):
            suffix = "" if final == url else f" -> {final}"
            print(f"{status:>5}  {url}{suffix}")
            if status == "ERROR" or status.startswith("5"):
                failures += 1

    print(f"done: {len(urls)} checked, {failures} network/server failure(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
