#!/usr/bin/python3


def update_inventory(inv, item, quantity):
    assert quantity >= 0, "Cannot update inventory with a negative quantity"

    inv[item] = quantity


if __name__ == "__main__":
    inv = {}

    update_inventory(inv, "Hammer", 5)
    update_inventory(inv, "Nails", 200)
    update_inventory(inv, "Screwdriver", 10)
    update_inventory(inv, "Screws", -100)

    total_items = 0
    for quantity in inv.values():
        total_items += quantity

    print("The total number of items is", total_items)
        

