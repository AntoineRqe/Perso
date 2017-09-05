# -*- coding: utf-8 -*-

import os
from toolbox import *

netboxe_url = "https://www.netboxe.fr/"
database_relative_path = os.path.join(os.getcwd(), "Datas", "netboxe_urls.db")

max_depth_level = 3


def main():

    # -----------------
    # Retrieve all Urls
    # -----------------

    if not os.path.exists(os.path.join(os.getcwd(), "Datas")):
        os.mkdir(os.path.join(os.getcwd(), "Datas"))
    if os.path.exists(database_relative_path):
        os.remove(database_relative_path)

    url_depth_dict = dict()
    url_lists = list()

    url_lists.append(netboxe_url)
    url_depth_dict[0] = url_lists

    for depth in range(1, (max_depth_level + 1)):
        url_lists = list()
        for url in url_depth_dict[depth-1]:
            url_lists += get_urls_from_page(url)
            url_lists = list(set(url_lists))

        url_depth_dict[depth] = url_lists

    del url_depth_dict[0]

    # ------------------------------
    # Write the data in the database
    # And close the database
    # ------------------------------

    conn, cur = create_database(database_relative_path)
    for depth, urls in url_depth_dict.items():
        for url in urls:
            entry = construct_database_entry(depth, url)
            cur.execute("INSERT INTO page_url(id, secure, depth, url)"
                        " VALUES(?,?,?,?)", entry)

    conn.commit()
    cur.close()
    conn.close()

    # ---------------------------------
    # Reopen the database and read them
    # ---------------------------------

    conn = sqlite3.connect(database_relative_path)
    cur = conn.cursor()

    cur.execute("SELECT * FROM page_url")

    print(pretty_print_database(cur.fetchall()))

    cur.close()
    conn.close()

if __name__ == "__main__":
    main()
