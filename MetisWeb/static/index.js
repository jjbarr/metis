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
    const outer = document.createElement('ul');
    const unselected = document.createElement('ul');    
    const selected = document.createElement('div');
    selected.classList.add('parts-list');
    unselected.classList.add('parts-list');
    const selected_label = document.createElement('h2');
    selected_label.innerText = 'Your Parts';
    const unselected_label = document.createElement('h2');
    unselected_label.innerText = 'Our Parts';
    outer.appendChild(selected_label);
    outer.appendChild(selected);
    outer.appendChild(document.createElement('hr'));
    outer.appendChild(unselected_label);
    outer.appendChild(unselected);
    db.filter((e) => e.name.match(filter) || e.selected)
        .forEach((e) => {
            const elt = document.createElement('li');
            elt.classList.add('part');
            elt.innerText = e.name;
            elt.addEventListener('click', () => {
                e.selected = !e.selected;
                serverChangeState(e);
                refreshList(db, filter);
            });
            (e.selected? selected:unselected).appendChild(elt);
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
    })();
});

