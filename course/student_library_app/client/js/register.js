document.addEventListener("DOMContentLoaded", () => {
    const registerForm = document.getElementById("register-form");
    const messageElement = document.getElementById("message");
  
    // Если форма не найдена, выходим
    if (!registerForm) return;
  
    registerForm.addEventListener("submit", async (event) => {
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
          messageElement.innerHTML = `
            <p class="success">
              Registration successful! 
              <a href="login.html">Login here</a>
            </p>`;
        } else {
          messageElement.innerHTML = `<p class="error">${data.detail}</p>`;
        }
      } catch (error) {
        messageElement.innerHTML = `<p class="error">An error occurred: ${error.message}</p>`;
      }
    });
  });
  