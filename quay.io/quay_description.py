#!/usr/bin/python3

import json
import requests
import argparse
from sys import stderr, stdout, stdin
from urllib.parse import urlparse
from argparse import ArgumentTypeError

def is_a_url(arg):
    if arg[-1] != "/":
        arg+="/"
    url = urlparse(arg)
    if all((url.scheme, url.netloc, url.path)):
        return arg

    raise ArgumentTypeError(f'not a valid url: {arg}')

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--server",   "-s", help="quay server url", type=is_a_url, required=True)
    parser.add_argument("--repo",     "-r", help="quay repository path", type=str, required=True)
    parser.add_argument("--token",    "-t", help="OAuth token to use", type=str, required=True)
    parser.add_argument("--file",     "-f", help="description file", type=str, required=False)
    parser.add_argument("--mode",     "-m", help="read or write the descriptionn", type=str, choices=['read', 'write'], default='read')
    args = parser.parse_args()

    # Authorization: Bearer $TOKEN" https://quay.io/api/v1/repository/gonoph/eatmemory | jq -r .description > description.md ^C
    headers={
            "Authorization": f"Bearer {args.token}",
            "Content-Type": "application/json"
            }
    url = f'{args.server}api/v1/repository/{args.repo}'

    if args.mode == 'read':
        do_read(url, headers, args)

    if args.mode == 'write':
        do_write(url, headers, args)

def do_read(url, headers, args):
    outf=stdout
    outf_file = 'stdout'
    if args.file:
        outf=open(args.file, "w")
        outf_file = args.file

    print(f'{args.mode} from {url}', file=stderr)
    print(f'+ sending description to {outf_file}', file=stderr)

    with outf:
        r = requests.get(url, headers=headers)
        r.raise_for_status()
        print(r.json()['description'], file=outf)

def do_write(url, headers, args):
    inf=stdin
    inf_file = 'stdin'
    description=''
    if args.file:
        inf=open(args.file, 'r')
        inf_file=args.file

    print(f'{args.mode} from {url}', file=stderr)
    print(f'+ pulling description from {inf_file}', file=stderr)
    # print(f'+ headers: {headers}', file=stderr)

    with inf:
        description=inf.read()
        inf.close()

    data=dict(description=description)
    r = requests.put(url, headers=headers, data=json.dumps(data))
    print(r.text)
    r.raise_for_status()

if __name__ == "__main__":
    main()
