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
    //(async () => {
        //await 
       
        body=JSON.stringify(item);
        // alert(body);

        fetch('/change_led', {
            method: 'POST',
            headers: {'Content-Type':'application/json'},
            body,
        });
        //right now we just... won't deal with errors.
        //we're talking to localhost
    //})();
}

function refreshList(db, filter) {
    const unselected_mount = document.getElementById('unselected-parts-mount');
    const selected_mount = document.getElementById('selected-parts-mount');
    const unselected = document.createElement('ul');
    const selected = document.createElement('ul');
    selected.classList.add('parts-list');
    unselected.classList.add('parts-list');
    db.filter((e) => e.name.match(filter) || e.selected)
        .forEach((e) => {
            const elt = document.createElement('li');
            elt.classList.add('part');
            elt.innerText = e.name ;//+ ' (' + e.col+' ,'+e.row + ')';
            elt.addEventListener('click', () => {
                
                e.selected = !!!e.selected;
                e.selected_bypass = e.selected ;
                serverChangeState(e);
                refreshList(db, filter);
            });
            (e.selected? selected:unselected).appendChild(elt);
        });
    while(unselected_mount.firstChild)
        unselected_mount.removeChild(unselected_mount.firstChild);
    unselected_mount.appendChild(unselected);
    while(selected_mount.firstChild)
        selected_mount.removeChild(selected_mount.firstChild);
    selected_mount.appendChild(selected);
}

document.addEventListener('DOMContentLoaded', () => {
    (async () => {
        await fetch('/reset_leds', {method: 'POST'});
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

