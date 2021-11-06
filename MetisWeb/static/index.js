/*let db = [
    {
        name: "part name",
        shelf_id: 11283,
        row: 0,
        col: 0,
        selected: false
    },
    {
        name: "another part",
        shelf_id: 1111,
        row: 1,
        col: 1,
        selected: false
    },
    {
        name: "yet another part",
        shelf_id: 1111,
        row: 2,
        col: 1,
        selected: false
    },
];*/

function serverChangeState(item) {
    (async () => {
        await fetch('/change_led', {
            method: 'POST',
            headers: {'Content-Type':'application/json'},
            body: JSON.stringify(item)
        });
        //right now we just... won't deal with errors.
        //we're talking to localhost
    })();
}

function refreshList(db, filter) {
    const mount = document.getElementById('list-mount');
    const outer = document.createElement('div');
    outer.classList.add();
    db.filter((e) => e.name.match(filter))
        .forEach((e) => {
            const elt = document.createElement('div');
            elt.classList.add('part');
            elt.innerText = e.name;
            if(e.selected) elt.classList.add('selected');
            elt.addEventListener('click', () => {
                e.selected = !e.selected;
                elt.classList.toggle('selected');
                serverChangeState(e);
            });
            outer.appendChild(elt);
        });
    while(mount.firstChild) mount.removeChild(mount.firstChild);
    mount.appendChild(outer);
}

document.addEventListener('DOMContentLoaded', () => {
    (async () => {
        let db = await (await fetch('/get_db')).json();
        db.sort((a,b) => a.name.localeCompare(b));
        document.getElementById('searchbox')
            .addEventListener('keydown', (e) => {
                if(e.keyCode == 13) e.preventDefault(); //enter shouln't refresh
            });
        document.getElementById('searchbox').addEventListener('keyup', (e) => {
            refreshList(db, document.getElementById('searchbox').value || '.*');
        });
        document.getElementById('reset').addEventListener('click', () => {
            document.getElementById('searchbox').value = '';
            db.forEach((e) => e.selected = false);
            refreshList(db, '.*');
            (async () => {
                await fetch('/reset_leds', {method: 'POST'});
                //again, no error handling.
            })();
        });
        refreshList(db, '.*');
    });
});

