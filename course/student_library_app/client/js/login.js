document.getElementById("login-btn").addEventListener("click", async () => {
  const username = document.getElementById("username").value;
  const password = document.getElementById("password").value;

  try {
    const response = await fetch("http://127.0.0.1:8000/login/", {
      method: "POST",
      headers: {
        "Content-Type": "application/x-www-form-urlencoded",
      },
      body: `username=${username}&password=${password}`,
    });

    if (!response.ok) {
      throw new Error("Invalid credentials");
    }

    const data = await response.json();
    localStorage.setItem("token", data.access_token);
    localStorage.setItem("role", data.role);
    localStorage.setItem("username", username);

    if (data.role === "teacher") {
      window.location.href = "../pages/teacher_panel.html";
    } else {
      window.location.href = "../pages/library.html";
    }
  } catch (error) {
    console.error(error);
    document.getElementById("error-message").innerText = "Login failed. Please try again.";
  }
});
