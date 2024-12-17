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

    // Получение данных пользователя
    const userResponse = await fetch("http://127.0.0.1:8000/users/me/", {
      headers: {
        Authorization: `Bearer ${data.access_token}`,
      },
    });

    const user = await userResponse.json();
    console.log("User data:", user); // Диагностика

    if (user.role === "teacher") {
      window.location.href = "../pages/teacher_panel.html";
    } else {
      window.location.href = "../pages/library.html";
    }
  } catch (error) {
    console.error(error);
    alert("Login failed");
  }
});
