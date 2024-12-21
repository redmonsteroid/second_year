document.addEventListener("DOMContentLoaded", () => {
  console.log("Teacher panel loaded...");
  loadBooks();

  const addBookForm = document.getElementById("add-book-form");
  if (addBookForm) {
    addBookForm.addEventListener("submit", handleAddBook);
  }
});

/* ---------- Полезные функции-хелперы для валидации (пример из предыдущего ответа) ---------- */
function sanitizeOrNA(value) {
  const trimmed = value.trim();
  return trimmed === "" ? "N/A" : trimmed;
}

function validateRequired(value, fieldName) {
  const trimmed = value.trim();
  if (!trimmed) {
    return `Please provide a valid ${fieldName}. This field cannot be empty or only spaces.`;
  }
  return "";
}

function validatePublicationYear(value) {
  const trimmed = value.trim();
  if (!trimmed) {
    return { year: null, error: "" };
  }
  const parsed = parseInt(trimmed, 10);
  if (isNaN(parsed)) {
    return { year: null, error: "Publication year must be a valid number (4 digits)." };
  }
  if (trimmed.length !== 4) {
    return { year: null, error: "Publication year must be a 4-digit number." };
  }
  if (parsed < 0 || parsed > 2025) {
    return { year: null, error: "Publication year must be between 0 and 2025." };
  }
  return { year: parsed, error: "" };
}
/* ----------------------------------------------------------------------------------------- */

async function handleAddBook(event) {
  event.preventDefault();

  clearAllErrors();

  // Смотрим, есть ли токен
  const token = localStorage.getItem("token");
  if (!token) {
    alert("Unauthorized. Please login.");
    return;
  }

  // Собираем значения
  const titleVal = document.getElementById("book-title").value;
  const downloadVal = document.getElementById("book-download-link").value;
  const cityVal = document.getElementById("book-publication-city").value;
  const publisherVal = document.getElementById("book-publisher").value;
  const yearVal = document.getElementById("book-publication-year").value;
  const pageCountVal = document.getElementById("book-page-count").value;
  const additionalInfoVal = document.getElementById("book-additional-info").value;

  // Валидация заголовка
  const titleError = validateRequired(titleVal, "Book Title");
  if (titleError) {
    showError("book-title-error", titleError);
  }

  // Валидация года
  const { year, error: yearError } = validatePublicationYear(yearVal);
  if (yearError) {
    showError("book-publication-year-error", yearError);
  }

  // Преобразуем остальные поля (N/A, если пусто)
  const finalDownloadLink = sanitizeOrNA(downloadVal);
  const finalCity = sanitizeOrNA(cityVal);
  const finalPublisher = sanitizeOrNA(publisherVal);
  const finalAdditionalInfo = sanitizeOrNA(additionalInfoVal);

  // Page count (необязательное, если не пусто — пытаемся прочитать)
  let finalPageCount = null;
  const pageTrim = pageCountVal.trim();
  if (pageTrim !== "") {
    const parsedPage = parseInt(pageTrim, 10);
    if (!isNaN(parsedPage) && parsedPage > 0) {
      finalPageCount = parsedPage;
    }
  }

  // Обработка авторов
  const authors = getAuthors();
  authors.forEach((author) => {
    author.last_name = sanitizeOrNA(author.last_name);
    author.first_name = sanitizeOrNA(author.first_name);
    author.middle_name = sanitizeOrNA(author.middle_name);
  });

  // Если есть ошибки — прекращаем
  if (titleError || yearError) {
    console.log("Validation failed. Title error:", titleError, "Year error:", yearError);
    return;
  }

  // Если валидация прошла
  console.log("Submitting new book with data:", {
    title: titleVal.trim(),
    download_link: finalDownloadLink,
    publication_city: finalCity,
    publisher: finalPublisher,
    publication_year: year,
    page_count: finalPageCount,
    additional_info: finalAdditionalInfo,
    authors
  });

  try {
    const response = await fetch("http://127.0.0.1:8000/books/", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${token}`,
      },
      body: JSON.stringify({
        title: titleVal.trim(),
        download_link: finalDownloadLink,
        publication_city: finalCity,
        publisher: finalPublisher,
        publication_year: year,
        page_count: finalPageCount,
        additional_info: finalAdditionalInfo,
        authors,
      }),
    });

    if (!response.ok) {
      throw new Error("Failed to add book");
    }

    console.log("Book added successfully!");
    document.getElementById("add-book-form").reset();
    loadBooks();
  } catch (error) {
    console.error(error);
    alert("Error adding book");
  }
}

function clearAllErrors() {
  const errorSpans = document.querySelectorAll(".error-message");
  errorSpans.forEach(span => {
    span.textContent = "";
  });
}

function showError(spanId, message) {
  const span = document.getElementById(spanId);
  if (span) {
    span.textContent = message;
  }
}

// Добавление/удаление авторов
function addAuthorField() {
  const container = document.getElementById("authors-container");
  const div = document.createElement("div");
  div.classList.add("author-entry");
  div.innerHTML = `
    <div class="input-group">
      <input type="text" class="author-last-name" placeholder="Last Name">
      <span class="error-message author-error"></span>
    </div>
    <div class="input-group">
      <input type="text" class="author-first-name" placeholder="First Name">
      <span class="error-message author-error"></span>
    </div>
    <div class="input-group">
      <input type="text" class="author-middle-name" placeholder="Middle Name">
      <span class="error-message author-error"></span>
    </div>
    <button type="button" class="remove-author-btn" onclick="removeAuthorField(this)">Remove</button>
  `;
  container.appendChild(div);
}

function removeAuthorField(button) {
  button.parentElement.remove();
}

function getAuthors() {
  const authorEntries = document.querySelectorAll(".author-entry");
  return Array.from(authorEntries).map(entry => {
    return {
      last_name: entry.querySelector(".author-last-name").value,
      first_name: entry.querySelector(".author-first-name").value,
      middle_name: entry.querySelector(".author-middle-name").value,
    };
  });
}

/* ------------------ КНИГИ ------------------ */
async function loadBooks() {
  const token = localStorage.getItem("token");
  if (!token) {
    alert("Unauthorized. Please login.");
    window.location.href = "login.html";
    return;
  }

  console.log("Loading books. Token found:", token);

  try {
    const response = await fetch("http://127.0.0.1:8000/books/", {
      headers: { Authorization: `Bearer ${token}` },
    });

    if (!response.ok) {
      throw new Error("Failed to load books");
    }

    const books = await response.json();
    console.log("Books received from server:", books);

    displayBooks(books);
  } catch (error) {
    console.error(error);
    alert("Error loading books");
  }
}

function displayBooks(books) {
  const bookList = document.getElementById("book-list");
  if (!bookList) return;

  bookList.innerHTML = "";

  // Отладка: что лежит в localStorage
  const currentUsername = localStorage.getItem("username");
  console.log("currentUsername from localStorage:", currentUsername);

  // Фильтруем по owner_username
  const teacherBooks = books.filter(book => {
    // Отладка: у каждой книги смотрим, какое поле у владельца
    console.log(`Book ID: ${book.id}, Title: ${book.title}, OwnerUsername: ${book.owner_username}`);
    return book.owner_username === currentUsername;
  });

  console.log("Filtered teacherBooks:", teacherBooks);

  teacherBooks.forEach(book => {
    const li = document.createElement("li");
    li.innerHTML = `
      <strong>Title:</strong> ${book.title || "N/A"} <br>
      <strong>Authors:</strong> ${
        book.authors && book.authors.length > 0
          ? book.authors
              .map(
                (a) =>
                  `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`
              )
              .join(", ")
          : "N/A"
      } <br>
      <strong>Publisher:</strong> ${book.publisher || "N/A"} <br>
      <strong>Publication City:</strong> ${book.publication_city || "N/A"} <br>
      <strong>Year:</strong> ${book.publication_year || "N/A"} <br>
      <strong>Pages:</strong> ${book.page_count || "N/A"} <br>
      <strong>Additional Info:</strong> ${book.additional_info || "N/A"} <br>
      <strong>Download Link:</strong> ${
        book.download_link
          ? `<a href="${book.download_link}" target="_blank">${book.download_link}</a>`
          : "N/A"
      } <br>
      <button class="delete-btn" onclick="deleteBook(${book.id})">Delete</button>
    `;
    bookList.appendChild(li);
  });
}

/* ------------------ УДАЛЕНИЕ ------------------ */
async function deleteBook(bookId) {
  const token = localStorage.getItem("token");
  if (!token) {
    alert("Unauthorized. Please login.");
    window.location.href = "login.html";
    return;
  }

  console.log("Deleting book with ID:", bookId);

  try {
    const response = await fetch(`http://127.0.0.1:8000/books/${bookId}`, {
      method: "DELETE",
      headers: { Authorization: `Bearer ${token}` },
    });

    if (!response.ok) {
      throw new Error("Failed to delete book");
    }

    console.log("Book deleted successfully:", bookId);
    loadBooks();
  } catch (error) {
    console.error(error);
    alert("Error deleting book");
  }
}

/* ------------------ ЛОГОУТ ------------------ */
function logout() {
  localStorage.removeItem("token");
  localStorage.removeItem("username");
  console.log("Logged out. Redirecting to login...");
  window.location.href = "login.html";
}
