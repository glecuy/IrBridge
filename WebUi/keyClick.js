
function senKeyName(code) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      //document.getElementById("demo").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "KeyName", true);
  xhttp.setRequestHeader("Value", code );
  xhttp.send();
}


function keyClick(value) {
  document.getElementById("demo").innerHTML = "Key = " + value;
  senKeyName(value);
}
