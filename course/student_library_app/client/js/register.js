document.getElementById("register-form").addEventListener("submit", async function (event) {
    event.preventDefault();

    const username = document.getElementById("username").value;
    const password = document.getElementById("password").value;
    const role = document.getElementById("role").value;

    try {
        const response = await fetch("http://127.0.0.1:8000/register/", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({ username, password, role }),
        });

        const data = await response.json();

        if (response.ok) {
            document.getElementById("message").innerHTML = `<p class="success">Registration successful! <a href="../index.html">Login here</a></p>`;
        } else {
            document.getElementById("message").innerHTML = `<p class="error">${data.detail}</p>`;
        }
    } catch (error) {
        document.getElementById("message").innerHTML = `<p class="error">An error occurred: ${error.message}</p>`;
    }
});
