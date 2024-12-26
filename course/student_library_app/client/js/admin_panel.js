document.addEventListener("DOMContentLoaded", () => {
    const token = localStorage.getItem("token");
    const role = localStorage.getItem("role");
  
    // Проверка роли
    if (!token || role !== "admin") {
      alert("Access denied. You are not an admin.");
      window.location.href = "../pages/library.html"; 
      return;
    }
  
    loadAllBooks();
  });
  
  /**
   * Загрузка всех книг
   */
  async function loadAllBooks() {
    const token = localStorage.getItem("token");
    if (!token) {
      alert("Unauthorized. Please login.");
      window.location.href = "login.html";
      return;
    }
  
    console.log("Admin panel: loading all books...");
  
    try {
      const response = await fetch("http://127.0.0.1:8000/books/", {
        headers: { Authorization: `Bearer ${token}` },
      });
  
      if (!response.ok) {
        throw new Error("Failed to load books");
      }
  
      const books = await response.json();
      console.log("Admin: books received from server:", books);
  
      displayBooks(books);
    } catch (error) {
      console.error(error);
      alert("Error loading books");
    }
  }
  
  /**
   * Отображаем все книги в списке, с кнопками Delete
   */
  function displayBooks(books) {
    const bookList = document.getElementById("book-list");
    if (!bookList) return;
    bookList.innerHTML = "";
  
    books.forEach((book) => {
      // Собираем строки для издательства / авторов
      const publisherName = book.publisher_rel ? book.publisher_rel.name : "N/A";
      const publisherCity = book.publisher_rel ? book.publisher_rel.city : "N/A";
  
      let authorsText = "N/A";
      if (book.authors && book.authors.length > 0) {
        authorsText = book.authors
          .map(a => 
            `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`
          )
          .join(", ");
      }
  
      const li = document.createElement("li");
      li.innerHTML = `
        <strong>Title:</strong> ${book.title || "N/A"} <br>
        <strong>Authors:</strong> ${authorsText} <br>
        <strong>Publisher:</strong> ${publisherName} <br>
        <strong>City:</strong> ${publisherCity} <br>
        <strong>Year:</strong> ${book.publication_year ?? "N/A"} <br>
        <strong>Pages:</strong> ${book.page_count ?? "N/A"} <br>
        <strong>Additional Info:</strong> ${book.additional_info || "N/A"} <br>
        <strong>Download Link:</strong> ${
          book.download_link
            ? `<a href="${book.download_link}" target="_blank">${book.download_link}</a>`
            : "N/A"
        } <br>
        <strong>Owner (Teacher/Student):</strong> ${book.owner_username || "N/A"} <br>
        
        <button class="delete-btn" onclick="deleteBook(${book.id})">Delete</button>
      `;
      bookList.appendChild(li);
    });
  }
  
  /**
   * Удаление книги (admin может удалять любые)
   */
  async function deleteBook(bookId) {
    const token = localStorage.getItem("token");
    if (!token) {
      alert("Unauthorized. Please login.");
      window.location.href = "login.html";
      return;
    }
  
    console.log("Admin: deleting book with ID:", bookId);
  
    try {
      const response = await fetch(`http://127.0.0.1:8000/books/${bookId}`, {
        method: "DELETE",
        headers: { Authorization: `Bearer ${token}` },
      });
  
      if (!response.ok) {
        throw new Error("Failed to delete book");
      }
  
      console.log("Book deleted by admin successfully:", bookId);
      loadAllBooks(); // Перезагружаем список
    } catch (error) {
      console.error(error);
      alert("Error deleting book");
    }
  }
  
  /**
   * Выход
   */
  function logout() {
    localStorage.removeItem("token");
    localStorage.removeItem("username");
    localStorage.removeItem("role");
    console.log("Logged out. Redirecting to login...");
    window.location.href = "login.html";
  }
  