document.addEventListener("DOMContentLoaded", () => {
    loadBooks();
    document.getElementById("add-book-form").addEventListener("submit", addBook);
  });
  
  // Функция для добавления нового поля для автора
  function addAuthorField() {
    const container = document.getElementById("authors-container");
    const div = document.createElement("div");
    div.classList.add("author-entry");
    div.innerHTML = `
      <input type="text" class="author-last-name" placeholder="Last Name">
      <input type="text" class="author-first-name" placeholder="First Name">
      <input type="text" class="author-middle-name" placeholder="Middle Name">
      <button type="button" onclick="removeAuthorField(this)">Remove</button>
    `;
    container.appendChild(div);
  }
  
  // Функция для удаления поля автора
  function removeAuthorField(button) {
    button.parentElement.remove();
  }
  
  // Сбор данных об авторах
  function getAuthors() {
    const authorEntries = document.querySelectorAll(".author-entry");
    return Array.from(authorEntries).map((entry) => {
      return {
        last_name: entry.querySelector(".author-last-name").value,
        first_name: entry.querySelector(".author-first-name").value,
        middle_name: entry.querySelector(".author-middle-name").value,
      };
    });
  }
  
  // Добавление новой книги
  async function addBook(event) {
    event.preventDefault();
  
    const token = localStorage.getItem("token");
    const title = document.getElementById("book-title").value;
    const download_link = document.getElementById("book-download-link").value;
    const publication_city = document.getElementById("book-publication-city").value;
    const publisher = document.getElementById("book-publisher").value;
    const publication_year = parseInt(document.getElementById("book-publication-year").value);
    const page_count = parseInt(document.getElementById("book-page-count").value);
    const additional_info = document.getElementById("book-additional-info").value;
    const authors = getAuthors();
  
    try {
      const response = await fetch("http://127.0.0.1:8000/books/", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify({
          title,
          download_link,
          publication_city,
          publisher,
          publication_year,
          page_count,
          additional_info,
          authors,
        }),
      });
  
      if (!response.ok) {
        throw new Error("Failed to add book");
      }
  
      document.getElementById("add-book-form").reset();
      loadBooks();
    } catch (error) {
      console.error(error);
      alert("Error adding book");
    }
  }
  
  // Загрузка книг
  async function loadBooks() {
    const token = localStorage.getItem("token");
    if (!token) {
      alert("Unauthorized. Please login.");
      window.location.href = "login.html";
      return;
    }
  
    try {
      const response = await fetch("http://127.0.0.1:8000/books/", {
        headers: { Authorization: `Bearer ${token}` },
      });
  
      if (!response.ok) {
        throw new Error("Failed to load books");
      }
  
      const books = await response.json();
      displayBooks(books);
    } catch (error) {
      console.error(error);
      alert("Error loading books");
    }
  }
  
  // Отображение книг
  function displayBooks(books) {
    const bookList = document.getElementById("book-list");
    bookList.innerHTML = "";
  
    books.forEach((book) => {
      const li = document.createElement("li");
      li.innerHTML = `
        <strong>Title:</strong> ${book.title} <br>
        <strong>Authors:</strong> ${book.authors.length > 0 ? book.authors.map(a => `${a.last_name} ${a.first_name} ${a.middle_name || ''}`).join(", ") : "N/A"} <br>
        <strong>Publisher:</strong> ${book.publisher || "N/A"} <br>
        <strong>Publication City:</strong> ${book.publication_city || "N/A"} <br>
        <strong>Year:</strong> ${book.publication_year || "N/A"} <br>
        <strong>Pages:</strong> ${book.page_count || "N/A"} <br>
        <strong>Additional Info:</strong> ${book.additional_info || "N/A"} <br>
        <strong>Download Link:</strong> ${book.download_link || "N/A"} <br>
        <button onclick="deleteBook(${book.id})">Delete</button>
      `;
      bookList.appendChild(li);
    });
  }
  
  
  // Удаление книги
  async function deleteBook(bookId) {
    const token = localStorage.getItem("token");
  
    try {
      const response = await fetch(`http://127.0.0.1:8000/books/${bookId}`, {
        method: "DELETE",
        headers: { Authorization: `Bearer ${token}` },
      });
  
      if (!response.ok) {
        throw new Error("Failed to delete book");
      }
  
      loadBooks();
    } catch (error) {
      console.error(error);
      alert("Error deleting book");
    }
  }
  
  // Функция для выхода из системы
  function logout() {
    localStorage.removeItem("token");
    window.location.href = "login.html";
  }
  